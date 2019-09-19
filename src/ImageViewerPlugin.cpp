#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <QInputDialog>

#include <vector>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
	_datasetNames(),
	_currentDataset(),
	_imageNames(),
	_currentImage(-1),
	_dimensionNames(),
	_currentDimension(),
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

		if (type == "MULTIPART")
			return ImageCollectionType::MultiPartSequence;
	}

	return ImageCollectionType::Undefined;
}

QString ImageViewerPlugin::currentImageName() const
{
	if (_currentImage >= 0)
		return imageFilePaths()[_currentImage];
	else
		return "";
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	PointsPlugin& points = pointsData();

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0; i < noDimensions; ++i)
		dimensionNames << points.dimNames[i];

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

int ImageViewerPlugin::noImages() const
{
	return imageFilePaths().size();
}

QSize ImageViewerPlugin::imageSize() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageSizes")) {
		if (imageCollectionType() == ImageCollectionType::Stack || imageCollectionType() == ImageCollectionType::Sequence) {
			return points.getProperty("imageSizes").toMap()["0"].toSize();
		}

		/*
		if (imageCollectionType() == "MULTIPART") {
			return points.getProperty("imageSizes").toMap()["0"].toSize();
		}
		*/
	}

	return QSize();
}

long ImageViewerPlugin::noPixels() const
{
	return imageSize().width() * imageSize().height();
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
		setDimensionNames(dimensionNames());
	}
}

void ImageViewerPlugin::computeDisplayImage()
{
	qDebug() << "Compute display image" << currentImageName();


	/*
	const auto imageSize			= imageSize();
	const auto noPixels				= noPixels();
	const auto imageCollectionType	= imageCollectionType();
	const auto noDisplayImages		= displayImages().size();
	const auto width				= imageSize().width();
	const auto height				= imageSize().height();

	
	if (QSize(texture("image").width(), texture("image").height()) != imageSize) {
		setupTextures();
	}

	auto& pointsData = _imageViewerPlugin->pointsData();
	auto& imageTextureData = textureData("image");

	if (imageCollectionType == ImageCollectionType::Sequence) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto imageOffset = displayImageId * noPixels;
					const auto pointId = imageOffset + pixelId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}

	if (imageCollectionType == ImageCollectionType::Stack) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}

	if (imageCollectionType == ImageCollectionType::MultiPartSequence) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}
	*/
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

Indices ImageViewerPlugin::displayImages() const
{
	auto displayImages = Indices();

	if (imageCollectionType() == ImageCollectionType::Sequence) {
		if (_averageImages) {
			if (hasSelection()) {
				displayImages = selection();
			}
			else {
				displayImages.resize(noImages());
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
	}

	if (imageCollectionType() == ImageCollectionType::Stack) {
		if (_averageImages) {
			displayImages.resize(noImages());
			std::iota(std::begin(displayImages), std::end(displayImages), 0);
		}
		else {
			if (_currentDimension >= 0) {
				displayImages = Indices({ static_cast<unsigned int>(_currentDimension) });
			}
			else {
				displayImages = Indices();
			}
		}
	}

	if (imageCollectionType() == ImageCollectionType::MultiPartSequence) {
	}

	return displayImages;
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
		
		if (type == "SEQUENCE" || type == "STACK" || type == "MULTIPART") {
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

	computeDisplayImage();
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