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
	_currentImageId(0),
	_dimensionNames(),
	_currentDimensionId(0),
	_averageImages(false),
	_window(0.f),
	_level(0.f)
{
	setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

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
	return ImageViewerPlugin::pixelId(imageSize, x, y) * 4;
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

	qDebug() << "Compute display image" << imageSize << pointsData.size();

	const auto noPointsPerDimension = this->noPointsPerDimension();

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
					auto pixelValue = 0.f;

					for (unsigned int displayImageId : displayImages) {
						const auto pointId = ImageViewerPlugin::sequencePixelCoordinateToPointId(imageSize, displayImageId, noPixels, x, y);

						pixelValue += pointsData[pointId];
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
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimensionId) });
			}

			const auto noDisplayDimensions	= displayDimensions.size();

			auto min = std::numeric_limits<int>::max();
			auto max = std::numeric_limits<int>::min();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId	= stackPixelCoordinateToPointId(imageSize, noImages, displayDimensionId, x, y);
						const auto value	= pointsData[pointId];

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
						const auto pointId = stackPixelCoordinateToPointId(imageSize, noImages, displayDimensionId, x, y);

						pixelValue += (pointsData[pointId] - min) / range;
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
				displayDimensions = Indices({ static_cast<unsigned int>(_currentDimensionId) });
			}

			const auto pixelOffset			= this->pixelOffset();
			const auto currentDimension		= this->_currentDimensionId;
			const auto noDisplayDimensions	= displayDimensions.size();

			auto min = std::numeric_limits<int>::max();
			auto max = std::numeric_limits<int>::min();

			for (std::int32_t x = 0; x < width; x++) {
				for (std::int32_t y = 0; y < height; y++) {
					for (unsigned int displayDimensionId : displayDimensions) {
						const auto pointId	= ImageViewerPlugin::multipartSequencePixelCoordinateToPointId(imageSize, noPointsPerDimension, pixelOffset, displayDimensionId, x, y);
						const auto value	= pointsData[pointId];

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
						const auto pointId = ImageViewerPlugin::multipartSequencePixelCoordinateToPointId(imageSize, noPointsPerDimension, pixelOffset, displayDimensionId, x, y);

						pixelValue += (pointsData[pointId] - min) / range;
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

void ImageViewerPlugin::computeSelectionImage()
{
	const auto imageSize = this->imageSize();
	const auto width	= imageSize.width();
	const auto height	= imageSize.height();
	const auto noPixels		= width * height;

	auto& pointsData = this->pointsData().getData();

	qDebug() << "Compute selection image" << imageSize << pointsData.size();

	auto selectionTextureData = TextureData();

	selectionTextureData.resize(noPixels * 4);

	if (hasSelection()) {
		switch (imageCollectionType())
		{
		case ImageCollectionType::Stack: {
			for (auto& index : selection())
			{
				const auto offset = index * 4;

				selectionTextureData[offset + 0] = 255;
				selectionTextureData[offset + 1] = 0;
				selectionTextureData[offset + 2] = 0;
				selectionTextureData[offset + 3] = 100;
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

				const auto offset = (selectionId - pointIndexStart) * 4;

				selectionTextureData[offset + 0] = 255;
				selectionTextureData[offset + 1] = 0;
				selectionTextureData[offset + 2] = 0;
				selectionTextureData[offset + 3] = 100;
			}
		}
		}
	}

	emit selectionImageChanged(imageSize, selectionTextureData);
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

float ImageViewerPlugin::window() const
{
	return _window;
}

void ImageViewerPlugin::setWindowLevel(const float& window, const float& level)
{
	if (window == _window && level == _level)
		return;

	qDebug() << "Set window/level" << window << level;

	_window = window;
	_level	= level;

	emit windowLevelChanged(_window, _level);
}

float ImageViewerPlugin::level() const
{
	return _level;
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

void ImageViewerPlugin::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
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

		case Qt::Key::Key_Space:
		{
			_imageViewerWidget->setInteractionMode(InteractionMode::Navigation);
			break;
		}

		default:
			break;
		}
	}

	QWidget::keyPressEvent(keyEvent);
}

void ImageViewerPlugin::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
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

		case Qt::Key::Key_Space:
		{
			_imageViewerWidget->setInteractionMode(InteractionMode::Selection);
			break;

		}
		default:
			break;
		}
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