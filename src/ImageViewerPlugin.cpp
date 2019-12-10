#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include <QDebug>
#include <QFileInfo>

#include "IndexSet.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
	_datasetNames(),
	_currentDatasetName(),
	_currentImageDataSet(nullptr),
	_imageNames(),
	_currentImageId(0),
	_dimensionNames(),
	_currentDimensionId(0),
	_averageImages(false),
	_selectionOpacity(0.6f)
{
	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	connect(this, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::currentDimensionIdChanged, this, &ImageViewerPlugin::computeDisplayImage);
	connect(this, &ImageViewerPlugin::averageImagesChanged, this, &ImageViewerPlugin::computeDisplayImage);
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);

	setMainLayout(layout);

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);
}

std::vector<std::uint32_t> ImageViewerPlugin::selection() const
{
	if (_currentImageDataSet == nullptr)
		return std::vector<std::uint32_t>();

	return _currentImageDataSet->indices();
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

ImageCollectionType ImageViewerPlugin::imageCollectionType() const
{
	if (_currentImageDataSet == nullptr)
		return ImageCollectionType::Undefined;

	return _currentImageDataSet->imageData().imageCollectionType();
}

bool ImageViewerPlugin::selectable() const
{
	return imageCollectionType() == ImageCollectionType::Stack;
}

QSize ImageViewerPlugin::imageSize() const
{
	if (_currentImageDataSet == nullptr)
		return QSize();

	return _currentImageDataSet->imageSize();
}

void ImageViewerPlugin::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
	if (_currentImageDataSet == nullptr)
		return;

	_currentImageDataSet->selectPixels(pixelCoordinates, selectionModifier);
}

std::uint32_t ImageViewerPlugin::noSelectedPixels()
{
	if (_currentImageDataSet == nullptr)
		return 0;

	return _currentImageDataSet->noSelectedPixels();
}

void ImageViewerPlugin::update()
{
	if (_currentImageDataSet == nullptr)
		return;

	qDebug() << "Update";

	auto imageFileNames = QStringList();

	foreach(const QString& imageFilePath, _currentImageDataSet->imageData().imageFilePaths())
	{
		imageFileNames << QFileInfo(imageFilePath).fileName();
	}

	if (imageCollectionType() == ImageCollectionType::Sequence) {
		auto imageNames = QStringList();

		if (hasSelection()) {
			auto images = QStringList();

			for (auto& index : selection())
			{
				images << QString("%1").arg(imageFileNames[index]);
			}

			const auto imagesString = images.join(", ");

			imageNames << imagesString;
		}
		else {
			if (_averageImages) {
				auto images = QStringList();

				for (std::uint32_t i = 0; i < _currentImageDataSet->imageData().noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (std::uint32_t i = 0; i < _currentImageDataSet->imageData().noImages(); i++) {
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
	if (_currentImageDataSet == nullptr)
		return;

	const auto windowLevel = _imageViewerWidget->windowLevel();

	switch (imageCollectionType()) {
		case ImageCollectionType::Sequence:
		{
			auto ids = std::vector<std::uint32_t>();
			
			if (hasSelection()) {
				const auto pointSelection = selection();
				ids = _averageImages ? pointSelection : std::vector<std::uint32_t>({ pointSelection.front() });
			}
			else
			{
				if (_averageImages) {
					ids.resize(_currentImageDataSet->imageData().noImages());
					std::iota(ids.begin(), ids.end(), 0);
				}
				else
				{
					ids = std::vector<std::uint32_t>({ static_cast<std::uint32_t>(_currentImageId) });
				}
			}
			
			auto image = _currentImageDataSet->sequenceImage(ids);

			emit displayImageChanged(image);
			break;
		}

		case ImageCollectionType::Stack:
		{
			auto image = _currentImageDataSet->stackImage(_currentDimensionId);

			emit displayImageChanged(image);
			break;
		}

		default:
			break;
	}
}

void ImageViewerPlugin::computeSelectionImage()
{
	if (_currentImageDataSet == nullptr)
		return;

	qDebug() << "Compute selection image";

	if (imageCollectionType() == ImageCollectionType::Stack) {
		emit selectionImageChanged(_currentImageDataSet->selectionImage(), _currentImageDataSet->selectionBounds(true));
	}
}

QStringList ImageViewerPlugin::datasetNames() const
{
	return _datasetNames;
}

ImageDataSet* ImageViewerPlugin::currentImageDataSet()
{
	return _currentImageDataSet;
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

	_currentImageDataSet = &_core->requestData<ImageDataSet>(_currentDatasetName);

	emit currentDatasetChanged(_currentDatasetName);

	update();

	setCurrentImageId(0);
	setCurrentDimensionId(0);

	computeSelectionImage();
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

	_averageImages = averageImages;

	qDebug() << "Set average images" << _averageImages;

	emit averageImagesChanged(_averageImages);

	computeDisplayImage();
	update();
}

float ImageViewerPlugin::selectionOpacity() const
{
	return _selectionOpacity;
}

void ImageViewerPlugin::setSelectionOpacity(const float& selectionOpacity)
{
	if (selectionOpacity == _selectionOpacity)
		return;

	_selectionOpacity = selectionOpacity;

	qDebug() << "Set selection opacity" << _selectionOpacity;

	emit selectionOpacityChanged(_selectionOpacity);
}

void ImageViewerPlugin::createSubsetFromSelection()
{
	if (_currentImageDataSet == nullptr)
		return;

	qDebug() << "Create subset from selection";

	_currentImageDataSet->createSubset();
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
	else
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