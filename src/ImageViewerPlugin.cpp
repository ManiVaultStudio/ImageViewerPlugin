#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <QInputDialog>

#include <vector>
#include <limits>

//#include "ImageViewWidget.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

using PixelCoordToPointIndex = std::function<int(int, int)>;

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
//	_windowWidget(nullptr),
	_datasetNames(),
	_currentDataset(),
	_imageNames(),
	_currentImageId(0),
	_dimensionNames(),
	_currentDimensionId(0),
	_averageImages(false)
{
	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);
	//_windowWidget		= new ImageViewWidget(this);

	connect(this, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentDimensionIdChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::averageImagesChanged, this, &ImageViewerPlugin::computeDisplayImage);
}

ImageViewerPlugin::~ImageViewerPlugin()
{
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();

	//addWidget(_imageViewerWidget);
	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	setLayout(layout);
}

PointsPlugin& ImageViewerPlugin::pointsData() const
{
	const auto& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDataset));

	return set.getData();
}

Indices ImageViewerPlugin::selection() const
{
	const auto& selection = dynamic_cast<const IndexSet&>(pointsData().getSelection());

	return selection.indices;
}

void ImageViewerPlugin::setSelection(Indices& indices)
{
	auto& selection = dynamic_cast<IndexSet&>(pointsData().getSelection());

	selection.indices = indices;

	_core->notifySelectionChanged(selection.getDataName());
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

int ImageViewerPlugin::noDimensions() const
{
	auto& points = pointsData();

	return points.getNumDimensions();
}

ImageCollectionType ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDataset.isEmpty())
		return ImageCollectionType::Undefined;

	auto& points = pointsData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type").toString();

		if (type == "STACK")
			return ImageCollectionType::Stack;

		if (type == "SEQUENCE")
			return ImageCollectionType::Sequence;

		if (type == "MULTI_PART_SEQUENCE")
			return ImageCollectionType::MultiPartSequence;
	}

	return ImageCollectionType::Undefined;
}

bool ImageViewerPlugin::selectable() const
{
	return imageCollectionType() == ImageCollectionType::Stack;// || imageCollectionType() == ImageCollectionType::MultiPartSequence
}

QString ImageViewerPlugin::currentImageFilePath() const
{
	if (_currentImageId >= 0)
		return imageFilePaths()[_currentImageId];
	else
		return "";
}

QString ImageViewerPlugin::currentImageFileName() const
{
	return QFileInfo(currentImageFilePath()).fileName();
}

QString ImageViewerPlugin::currentDimensionName() const
{
	if (_currentDimensionId < _dimensionNames.size())
		return _dimensionNames[_currentDimensionId];
	else
		return "";
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	auto& points = pointsData();

	QStringList dimensionNames;
	
	dimensionNames.reserve(points.getDimensionNames().size());
	std::copy(points.getDimensionNames().begin(), points.getDimensionNames().end(), std::back_inserter(dimensionNames));

	return dimensionNames;
}

QStringList ImageViewerPlugin::imageFilePaths() const
{
	auto& points = pointsData();

	if (points.hasProperty("imageFilePaths")) {
		return points.getProperty("imageFilePaths").toStringList();
	}

	return QStringList();
}

std::size_t ImageViewerPlugin::noImages() const
{
	return imageFilePaths().size();
}

std::size_t ImageViewerPlugin::noPointsPerDimension() const
{
	const auto& points = pointsData();

	const auto imageSizes = points.getProperty("imageSizes").toMap();

	std::size_t noPointsPerDimension = 0;

	foreach(const QString& key, imageSizes.keys()) {
		const auto size = imageSizes[key].toSize();

		noPointsPerDimension += size.width() * size.height();
	}

	return noPointsPerDimension;
}

std::size_t ImageViewerPlugin::pixelOffset() const
{
	PointsPlugin& points = pointsData();

	if (!points.hasProperty("imageSizes"))
		return 0;

	auto imageSizes = points.getProperty("imageSizes").toMap();

	std::size_t pixelOffset = 0;

	for (std::int32_t i = 0; i < _currentImageId; i++) {
		const auto key	= imageSizes.keys().at(i);
		const auto size = imageSizes[key].toSize();

		pixelOffset += size.width() * size.height();
	}

	return pixelOffset;
}

std::size_t ImageViewerPlugin::pixelId(const QSize& imageSize, const int& x, const int& y)
{
	return y * imageSize.width() + x;
}

std::size_t ImageViewerPlugin::pixelBufferOffset(const QSize& imageSize, const int& x, const int& y)
{
	return ImageViewerPlugin::pixelId(imageSize, x, y);
}

std::size_t ImageViewerPlugin::sequencePixelCoordinateToPointId(const QSize& imageSize, const std::int32_t& imageId, const std::int32_t& noPixels, const std::int32_t& x, const std::int32_t& y)
{
	const auto imageOffset = imageId * (imageSize.width() * imageSize.height());
	return imageOffset + ImageViewerPlugin::pixelId(imageSize, x, y);
}

std::size_t ImageViewerPlugin::stackPixelCoordinateToPointId(const QSize& imageSize, const std::int32_t& noDimensions, const std::int32_t& dimensionId, const std::int32_t& x, const std::int32_t& y)
{
	return ImageViewerPlugin::pixelId(imageSize, x, y) * noDimensions + dimensionId;
}

std::size_t ImageViewerPlugin::multipartSequencePixelCoordinateToPointId(const QSize& imageSize, const std::int32_t& noPointsPerDimension, const std::int32_t& pixelOffset, const std::int32_t& currentDimension, const std::int32_t& x, const std::int32_t& y)
{
	return  (currentDimension * noPointsPerDimension) + pixelOffset + ImageViewerPlugin::pixelId(imageSize, x, y);
}

QSize ImageViewerPlugin::imageSize() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageSizes")) {
		const auto imageSizes = points.getProperty("imageSizes").toMap();

		switch (imageCollectionType())
		{
			case ImageCollectionType::Sequence:
			case ImageCollectionType::Stack:
			{
				return imageSizes["0"].toSize();
				break;
			}

			case ImageCollectionType::MultiPartSequence:
			{
				return imageSizes[currentImageFilePath()].toSize();
				break;
			}

			default:
				break;
		}
	}

	return QSize();
}

void ImageViewerPlugin::update()
{
	qDebug() << "Update";

	auto imageFileNames = QStringList();

	foreach(const QString& imageFilePath, imageFilePaths())
	{
		imageFileNames << QFileInfo(imageFilePath).fileName();
	}

	if (imageCollectionType() == ImageCollectionType::Sequence) {
		auto imageNames = QStringList();

		if (hasSelection()) {
			auto images = QStringList();

			for (unsigned int index : selection())
			{
				images << QString("%1").arg(imageFileNames[index]);
			}

			const auto imagesString = images.join(", ");

			imageNames << imagesString;
		}
		else {
			if (_averageImages) {
				auto images = QStringList();

				for (int i = 0; i < noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (int i = 0; i < noImages(); i++) {
					imageNames << QString("%1").arg(imageFileNames[i]);
				}
			}
		}

		setImageNames(imageNames);
		setDimensionNames(QStringList());
	}

	if (imageCollectionType() == ImageCollectionType::Stack) {
		setImageNames(QStringList());

		auto dimensionNames = QStringList();

		if (_averageImages) {
			dimensionNames << imageFileNames.join(", ");
		}
		else {
			dimensionNames = imageFileNames;
		}

		setDimensionNames(dimensionNames);
	}

	if (imageCollectionType() == ImageCollectionType::MultiPartSequence) {
		auto imageNames = QStringList();

		if (_averageImages) {
			auto images = QStringList();

			for (int i = 0; i < noImages(); i++) {
				images << QString("%1").arg(imageFileNames[i]);
			}

			const auto imagesString = images.join(", ");

			imageNames << imagesString;
		}
		else {
			for (int i = 0; i < noImages(); i++) {
				imageNames << QString("%1").arg(imageFileNames[i]);
			}
		}

		setImageNames(imageNames);
		setDimensionNames(dimensionNames());
	}
}

void ImageViewerPlugin::computeDisplayImage()
{
	const auto imageSize	= this->imageSize();
	const auto width		= imageSize.width();
	const auto height		= imageSize.height();
	const auto noPixels		= width * height;
	const auto noImages		= this->noImages();

	auto& pointsData = this->pointsData().getData();

	qDebug() << "Compute display image" << imageSize;

	const auto noPointsPerDimension = this->noPointsPerDimension();

	auto image = std::make_unique<Image<std::uint16_t>>(imageSize.width(), imageSize.height());

	switch (imageCollectionType()) {
		case ImageCollectionType::Sequence: {
			auto displayImages = Indices();

			if (_averageImages) {
				if (hasSelection()) {
					displayImages = selection();
				}
				else {
					displayImages.resize(noImages);
					std::iota(std::begin(displayImages), std::end(displayImages), 0);
				}
			}
			else {
				if (_currentImageId >= 0) {
					displayImages = Indices({ static_cast<unsigned int>(_currentImageId) });
				}
				else {
					displayImages = Indices();
				}
			}

			const auto noDisplayImages = displayImages.size();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					auto pixelValue = 0.0;

					for (unsigned int displayImageId : displayImages) {
						const auto pointId = ImageViewerPlugin::sequencePixelCoordinateToPointId(imageSize, displayImageId, noPixels, x, y);

						pixelValue += pointsData[pointId];
					}

					pixelValue /= static_cast<float>(noDisplayImages);

					image->setPixel(x, y, static_cast<std::uint16_t>(pixelValue));
				}
			}

			break;
		}

		case ImageCollectionType::Stack: {
			auto displayDimensions = Indices();

			if (_averageImages) {
				displayDimensions.resize(noDimensions());
				std::iota(std::begin(displayDimensions), std::end(displayDimensions), 0);
			}
			else
			{
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimensionId) });
			}

			const auto noDisplayDimensions	= displayDimensions.size();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					auto pixelValue = 0.f;

					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId = stackPixelCoordinateToPointId(imageSize, noImages, displayDimensionId, x, y);

						pixelValue += pointsData[pointId];
					}

					pixelValue /= static_cast<float>(noDisplayDimensions);

					image->setPixel(x, y, static_cast<std::uint16_t>(pixelValue));
				}
			}

			break;
		}

		case ImageCollectionType::MultiPartSequence: {
			auto displayDimensions = Indices();

			if (_averageImages) {
				displayDimensions.resize(noDimensions());
				std::iota(std::begin(displayDimensions), std::end(displayDimensions), 0);
			}
			else
			{
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimensionId) });
			}

			const auto pixelOffset			= this->pixelOffset();
			const auto currentDimension		= this->_currentDimensionId;
			const auto noDisplayDimensions	= displayDimensions.size();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					auto pixelValue = 0.f;

					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId = ImageViewerPlugin::multipartSequencePixelCoordinateToPointId(imageSize, noPointsPerDimension, pixelOffset, displayDimensionId, x, y);

						pixelValue += pointsData[pointId];
					}

					pixelValue /= static_cast<float>(noDisplayDimensions);

					image->setPixel(x, y, static_cast<std::uint16_t>(pixelValue));
				}
			}
			
			//qDebug() << imageTextureData;

			break;
		}
	}

	image->computeMinMax();

	emit displayImageChanged(image);
}

void ImageViewerPlugin::computeSelectionImage()
{
	const auto imageSize	= this->imageSize();
	const auto width		= imageSize.width();
	const auto height		= imageSize.height();
	const auto noPixels		= width * height;

	auto& pointsData = this->pointsData().getData();

	qDebug() << "Compute selection image" << imageSize << pointsData.size();

	auto image = std::make_unique<Image<std::uint8_t>>(imageSize.width(), imageSize.height());

	if (hasSelection()) {
		switch (imageCollectionType())
		{
		case ImageCollectionType::Stack: {
			for (auto& index : selection())
			{
				const auto offset = index * 4;

				image->pixels()[index] = 255;
			}
		}

		case ImageCollectionType::MultiPartSequence: {
			const auto imageSize = this->imageSize();
			const auto pointIndexStart = pixelOffset();
			const auto pointIndexEnd = pointIndexStart + imageSize.width() * imageSize.height();
			
			//qDebug() << pointIndexStart << imageSize;
			
			for (auto& selectionId : selection())
			{
				if (selectionId < pointIndexStart || selectionId >= pointIndexEnd)
					continue;

				image->pixels()[selectionId - pointIndexStart] = 255;
			}
		}
		}
	}

	emit selectionImageChanged(image);
}

QString ImageViewerPlugin::currentDataset() const
{
	return _currentDataset;
}

void ImageViewerPlugin::setCurrentDataset(const QString& currentDataset)
{
	if (currentDataset == _currentDataset)
		return;

	qDebug() << "Set current data set";

	_currentDataset = currentDataset;

	emit currentDatasetChanged(_currentDataset);

	update();

	setCurrentImageId(0);
	setCurrentDimensionId(0);
}

auto ImageViewerPlugin::currentImageId() const
{
	return _currentImageId;
}

void ImageViewerPlugin::setCurrentImageId(const std::int32_t& currentImageId)
{
	if (currentImageId == _currentImageId)
		return;

	if (currentImageId < 0)
		return;

	qDebug() << "Set current image ID";

	_currentImageId = currentImageId;

	emit currentImageIdChanged(_currentImageId);

	computeSelectionImage();
}

auto ImageViewerPlugin::currentDimensionId() const
{
	return _currentDimensionId;
}

void ImageViewerPlugin::setCurrentDimensionId(const std::int32_t& currentDimensionId)
{
	if (currentDimensionId == _currentDimensionId)
		return;

	if (currentDimensionId < 0)
		return;

	qDebug() << "Set current dimension ID";

	_currentDimensionId = currentDimensionId;

	emit currentDimensionIdChanged(_currentDimensionId);

	update();
}

bool ImageViewerPlugin::averageImages() const
{
	return _averageImages;
}

void ImageViewerPlugin::setAverageImages(const bool& averageImages)
{
	if (averageImages == _averageImages)
		return;

	qDebug() << "Set average images" << averageImages;

	_averageImages = averageImages;

	emit averageImagesChanged(_averageImages);

	update();
}

void ImageViewerPlugin::setDatasetNames(const QStringList& datasetNames)
{
	_datasetNames = datasetNames;

	emit datasetNamesChanged(_datasetNames);
}

void ImageViewerPlugin::setImageNames(const QStringList& imageNames)
{
	if (imageNames == _imageNames)
		return;

	qDebug() << "Set image names";

	_imageNames = imageNames;

	emit imageNamesChanged(_imageNames);
}

void ImageViewerPlugin::setDimensionNames(const QStringList& dimensionNames)
{
	if (dimensionNames == _dimensionNames)
		return;

	qDebug() << "Set dimension names";

	_dimensionNames = dimensionNames;

	emit dimensionNamesChanged(_dimensionNames);
}

void ImageViewerPlugin::dataAdded(const QString dataset)
{
	qDebug() << "Data added" << dataset;

	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(dataset));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");
		
		if (type == "SEQUENCE" || type == "STACK" || type == "MULTI_PART_SEQUENCE") {
			setDatasetNames(_datasetNames << dataset);
			setCurrentDataset(dataset);
		}
	}
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
	qDebug() << "Data changed" << dataset;
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
	qDebug() << "Data removed" << dataset;
	
	_datasetNames.removeAt(_datasetNames.indexOf(dataset));

	emit datasetNamesChanged(_datasetNames);
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	qDebug() << "Selection changed" << dataset;

	update();

	if (imageCollectionType() == ImageCollectionType::Sequence)
		computeDisplayImage();

	computeSelectionImage();
}

QStringList ImageViewerPlugin::supportedDataKinds()
{
	QStringList supportedKinds;

	supportedKinds << "Points";
	
	return supportedKinds;
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}