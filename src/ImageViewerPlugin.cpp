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

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

using PixelCoordToPointIndex = std::function<int(int, int)>;

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
	_datasetNames(),
	_currentDataset(),
	_imageNames(),
	_currentImage(0),
	_dimensionNames(),
	_currentDimension(0),
	_averageImages(false)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	connect(this, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentImageChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentDimensionChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::averageImagesChanged, this, &ImageViewerPlugin::computeDisplayImage);
}

ImageViewerPlugin::~ImageViewerPlugin()
{
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	setLayout(layout);
}

PointsPlugin& ImageViewerPlugin::pointsData() const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDataset));

	return set.getData();
}

Indices ImageViewerPlugin::selection() const
{
	const IndexSet& selection = dynamic_cast<const IndexSet&>(pointsData().getSelection());

	return selection.indices;
}

void ImageViewerPlugin::setSelection(Indices& indices)
{
	IndexSet& selection = dynamic_cast<IndexSet&>(pointsData().getSelection());

	selection.indices = indices;

	_core->notifySelectionChanged(selection.getDataName());
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

int ImageViewerPlugin::noDimensions() const
{
	PointsPlugin& points = pointsData();

	return points.getNumDimensions();
}

ImageCollectionType ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDataset.isEmpty())
		return ImageCollectionType::Undefined;

	PointsPlugin& points = pointsData();

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

QString ImageViewerPlugin::currentImageFilePath() const
{
	if (_currentImage >= 0)
		return imageFilePaths()[_currentImage];
	else
		return "";
}

QString ImageViewerPlugin::currentImageFileName() const
{
	return QFileInfo(currentImageFilePath()).fileName();
}

QString ImageViewerPlugin::currentDimensionName() const
{
	if (_currentDimension < _dimensionNames.size())
		return _dimensionNames[_currentDimension];
	else
		return "";
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	PointsPlugin& points = pointsData();

	QStringList dimensionNames;
	
	dimensionNames.reserve(points.getDimensionNames().size());
	std::copy(points.getDimensionNames().begin(), points.getDimensionNames().end(), std::back_inserter(dimensionNames));

	return dimensionNames;
}

QStringList ImageViewerPlugin::imageFilePaths() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageFilePaths")) {
		return points.getProperty("imageFilePaths").toStringList();
	}

	return QStringList();
}

std::size_t ImageViewerPlugin::noImages() const
{
	return imageFilePaths().size();
}

std::size_t ImageViewerPlugin::pixelId(const QSize& imageSize, const int& x, const int& y)
{
	return y * imageSize.width() + x;
}

std::size_t ImageViewerPlugin::pixelBufferOffset(const QSize& imageSize, const int& x, const int& y)
{
	return ImageViewerPlugin::pixelId(imageSize, x, y) * 4;
}

std::size_t ImageViewerPlugin::sequenceCoordinateToPointId(const QSize& imageSize, const std::int32_t& imageId, const std::int32_t& noPixels, const std::int32_t& x, const std::int32_t& y)
{
	const auto imageOffset = imageId * (imageSize.width() * imageSize.height());
	return imageOffset + ImageViewerPlugin::pixelId(imageSize, x, y);
}

std::size_t ImageViewerPlugin::stackCoordinateToPointId(const QSize& imageSize, const std::int32_t& noImages, const std::int32_t& imageId, const std::int32_t& x, const std::int32_t& y)
{
	return ImageViewerPlugin::pixelId(imageSize, x, y) * noImages + imageId;
}

std::size_t ImageViewerPlugin::multipartSequenceCoordinateToPointId(const QSize& imageSize, const std::int32_t& noPointsPerDimension, const std::int32_t& imageOffset, const std::int32_t& currentDimension, const std::int32_t& x, const std::int32_t& y)
{
	return  (currentDimension * noPointsPerDimension) + imageOffset + ImageViewerPlugin::pixelId(imageSize, x, y);
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

	auto& pointsData = this->pointsData();

	qDebug() << "Compute display image" << imageSize << pointsData.getData().size();

	PointsPlugin& points = this->pointsData();

	const auto imageSizes = points.getProperty("imageSizes").toMap();

	int noPointsPerDimension = 0;

	foreach(const QString& key, imageSizes.keys()) {
		const auto size = imageSizes[key].toSize();

		noPointsPerDimension += size.width() * size.height();
	}

	const auto getPixelId = [width](int x, int y) {
		return y * width + x;
	};

	auto imageTextureData = TextureData();

	imageTextureData.resize(noPixels * 4);

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
				if (_currentImage >= 0) {
					displayImages = Indices({ static_cast<unsigned int>(_currentImage) });
				}
				else {
					displayImages = Indices();
				}
			}

			const auto noDisplayImages = displayImages.size();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					auto pixelValue = 0.f;

					for (unsigned int displayImageId : displayImages) {
						const auto pointId = ImageViewerPlugin::sequenceCoordinateToPointId(imageSize, displayImageId, noPixels, x, y);

						pixelValue += pointsData.getData()[pointId];
					}

					pixelValue /= static_cast<float>(noDisplayImages);

					const auto offset = ImageViewerPlugin::pixelBufferOffset(imageSize, x, y);

					imageTextureData[offset + 0] = pixelValue;
					imageTextureData[offset + 1] = pixelValue;
					imageTextureData[offset + 2] = pixelValue;
					imageTextureData[offset + 3] = 255;
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
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimension) });
			}

			const auto noDisplayDimensions	= displayDimensions.size();

			auto min = std::numeric_limits<int>::max();
			auto max = std::numeric_limits<int>::min();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId	= stackCoordinateToPointId(imageSize, noImages, displayDimensionId, x, y);
						const auto value	= pointsData.getData()[pointId];

						if (value < min)
							min = value;

						if (value > max)
							max = value;
					}
				}
			}

			const auto range = max - min;

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					auto pixelValue = 0.f;

					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId = stackCoordinateToPointId(imageSize, noImages, displayDimensionId, x, y);

						pixelValue += (pointsData.getData()[pointId] - min) / range;
					}

					pixelValue /= static_cast<float>(noDisplayDimensions);
					pixelValue *= 255.f;

					const auto offset = ImageViewerPlugin::pixelBufferOffset(imageSize, x, y);

					imageTextureData[offset + 0] = pixelValue;
					imageTextureData[offset + 1] = pixelValue;
					imageTextureData[offset + 2] = pixelValue;
					imageTextureData[offset + 3] = 255;
				}
			}

			auto image = QImage((uchar*)&imageTextureData[0], width, height, QImage::Format::Format_RGBA8888);

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
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimension) });
			}

			int imageOffset = 0;

			for (int i = 0; i < _currentImage; i++) {
				const auto key = imageSizes.keys().at(i);
				const auto size = imageSizes[key].toSize();

				imageOffset += size.width() * size.height();
			}

			const auto currentDimension = this->_currentDimension;

			const auto noDisplayDimensions = displayDimensions.size();

			auto min = std::numeric_limits<int>::max();
			auto max = std::numeric_limits<int>::min();

			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId	= ImageViewerPlugin::multipartSequenceCoordinateToPointId(imageSize, noPointsPerDimension, imageOffset, displayDimensionId, x, y);
						const auto value	= pointsData.getData()[pointId];

						if (value < min)
							min = value;

						if (value > max)
							max = value;
					}
				}
			}

			const auto range = max - min;

			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					auto pixelValue = 0.f;

					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId = ImageViewerPlugin::multipartSequenceCoordinateToPointId(imageSize, noPointsPerDimension, imageOffset, displayDimensionId, x, y);

						pixelValue += (pointsData.getData()[pointId] - min) / range;
					}

					pixelValue /= static_cast<float>(noDisplayDimensions);
					pixelValue *= 255.f;

					const auto offset = ImageViewerPlugin::pixelBufferOffset(imageSize, x, y);

					imageTextureData[offset + 0] = pixelValue;
					imageTextureData[offset + 1] = pixelValue;
					imageTextureData[offset + 2] = pixelValue;
					imageTextureData[offset + 3] = 255;
				}
			}
			
			break;
		}
	}

	emit displayImageChanged(imageSize, imageTextureData);
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

	setCurrentImage(0);
	setCurrentDimension(0);
}

auto ImageViewerPlugin::currentImage() const
{
	return _currentImage;
}

void ImageViewerPlugin::setCurrentImage(const int& currentImage)
{
	if (currentImage == _currentImage)
		return;

	if (currentImage < 0)
		return;

	qDebug() << "Set current data image";

	_currentImage = currentImage;

	emit currentImageChanged(_currentImage);
}

auto ImageViewerPlugin::currentDimension() const
{
	return _currentDimension;
}

void ImageViewerPlugin::setCurrentDimension(const int& currentDimension)
{
	if (currentDimension == _currentDimension)
		return;

	if (currentDimension < 0)
		return;

	qDebug() << "Set current dimension";

	_currentDimension = currentDimension;

	emit currentDimensionChanged(_currentDimension);

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
	computeDisplayImage();

	emit selectedPointsChanged();
}

void ImageViewerPlugin::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	switch (keyEvent->key())
	{
		case Qt::Key::Key_R:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Rectangle);
			break;
		}

		case Qt::Key::Key_B:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Brush);
			break;
		}

		case Qt::Key::Key_F:
		{
			_imageViewerWidget->setSelectionType(ImageViewerWidget::SelectionType::Freehand);
			break;
		}

		case Qt::Key::Key_Shift:
		{
			//if (_imageViewerWidget->selectionModifier() != ImageViewerWidget::SelectionModifier::Remove)
			_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Add);
			break;
		}

		case Qt::Key::Key_Control:
		{
			//if (_imageViewerWidget->selectionModifier() != ImageViewerWidget::SelectionModifier::Add)
			_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Remove);
			break;
		}

		case Qt::Key::Key_Alt:
		{
			_imageViewerWidget->setInteractionMode(ImageViewerWidget::InteractionMode::Navigation);
			break;
		}

		default:
			break;
	}

	QWidget::keyPressEvent(keyEvent);
}

void ImageViewerPlugin::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent->key();

	switch (keyEvent->key())
	{
		case Qt::Key::Key_Shift:
		case Qt::Key::Key_Control:
		{
			if (_imageViewerWidget->selectionType() != ImageViewerWidget::SelectionType::Brush) {
				_imageViewerWidget->setSelectionModifier(ImageViewerWidget::SelectionModifier::Replace);
			}
			
			break;
		}

		case Qt::Key::Key_Alt:
		{
			_imageViewerWidget->setInteractionMode(ImageViewerWidget::InteractionMode::Selection);
			break;

		}
		default:
			break;
	}

	QWidget::keyReleaseEvent(keyEvent);
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