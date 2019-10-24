#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "Set.h"
#include "ImageData/ImageData.h"

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
	_currentDatasetName(),
	_currentImageData(nullptr),
	_imageNames(),
	_currentImageId(0),
	_dimensionNames(),
	_currentDimensionId(0),
	_averageImages(false)
{
	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

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

Indices ImageViewerPlugin::selection() const
{
	if (_currentImageData == nullptr)
		return Indices();

	const auto& selection = dynamic_cast<const ImageDataSet&>(_currentImageData->getSelection());

	return selection.indices;
}

void ImageViewerPlugin::setSelection(Indices& indices)
{
	if (_currentImageData == nullptr)
		return;

	auto& selection = dynamic_cast<ImageDataSet&>(_currentImageData->getSelection());

	selection.indices = indices;

	_core->notifySelectionChanged(selection.getDataName());
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

int ImageViewerPlugin::noDimensions() const
{
	if (_currentImageData == nullptr)
		return 0;

	return _currentImageData->noDimensions();
}

ImageCollectionType ImageViewerPlugin::imageCollectionType() const
{
	if (_currentImageData == nullptr)
		return ImageCollectionType::Undefined;

	return _currentImageData->imageCollectionType();
}

bool ImageViewerPlugin::selectable() const
{
	return imageCollectionType() == ImageCollectionType::Stack;
}

QSize ImageViewerPlugin::imageSize() const
{
	if (_currentImageData == nullptr)
		return QSize();

	return _currentImageData->imageSize();
}

void ImageViewerPlugin::update()
{
	if (_currentImageData == nullptr)
		return;

	qDebug() << "Update";

	auto imageFileNames = QStringList();

	foreach(const QString& imageFilePath, _currentImageData->imageFilePaths())
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

				for (int i = 0; i < _currentImageData->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (int i = 0; i < _currentImageData->noImages(); i++) {
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
}

void ImageViewerPlugin::computeDisplayImage()
{
	const auto windowLevel = _imageViewerWidget->windowLevel();

	switch (imageCollectionType()) {
		case ImageCollectionType::Sequence:
		{
			const auto ids = std::vector<std::uint32_t>({ 0, 1, 2, 3 });

			auto image = _currentImageData->sequenceImage(ids, windowLevel.first, windowLevel.second);
			emit displayImageChanged(image);
			break;
		}

		case ImageCollectionType::Stack:
		{
			auto image = _currentImageData->stackImage(_currentDimensionId, windowLevel.first, windowLevel.second);
			emit displayImageChanged(image);
			break;
		}

		default:
			break;
	}
}

void ImageViewerPlugin::computeSelectionImage()
{
	/*
	const auto imageSize	= this->imageSize();
	const auto width		= imageSize.width();
	const auto height		= imageSize.height();
	const auto noPixels		= width * height;

	auto& pointsData = this->pointsData().getData();

	qDebug() << "Compute selection image" << imageSize << pointsData.size();

	auto image = std::make_unique<Image>(imageSize.width(), imageSize.height());

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
	*/
}

QString ImageViewerPlugin::currentDatasetName() const
{
	return _currentDatasetName;
}

void ImageViewerPlugin::setCurrentDatasetName(const QString& currentDatasetName)
{
	if (currentDatasetName == _currentDatasetName)
		return;

	qDebug() << "Set current data set name" << currentDatasetName;

	_currentDatasetName = currentDatasetName;

	ImageDataSet& imageDataSet = dynamic_cast<ImageDataSet&>(_core->requestSet(_currentDatasetName));

	_currentImageData = &imageDataSet.getData();

	emit currentDatasetChanged(_currentDatasetName);

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

	setDatasetNames(_datasetNames << dataset);
	setCurrentDatasetName(dataset);
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

	supportedKinds << "Image Data";
	
	return supportedKinds;
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}