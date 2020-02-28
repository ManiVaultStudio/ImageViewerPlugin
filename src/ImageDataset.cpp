#include "DatasetsModel.h"
#include "ImageDataset.h"
#include "ImageLayer.h"

#include <QFileInfo>
#include <QMenu>
#include <QDebug>

ImageDataset::ImageDataset(ImageDatasetsModel* imageDatasets, const QString& name, Images* images) :
	QObject(),
	_name(name),
	_dataset(images),
	_imageNames(),
	_currentImageIndex(0),
	_dimensionNames(),
	_currentDimensionIndex(0),
	_average(false),
	_selectionOpacity(0.5f),
	_imageLayers()
{
	auto imageFileNames = QStringList();

	for (const auto& imageFilePath : _dataset->imageFilePaths())
	{
		imageFileNames << QFileInfo(imageFilePath).fileName();
	}

	if (_dataset->imageCollectionType() == ImageCollectionType::Sequence) {
		if (hasSelection()) {
			auto images = QStringList();

			for (auto& index : selection())
			{
				images << QString("%1").arg(imageFileNames[index]);
			}

			const auto imagesString = images.join(", ");

			_imageNames << imagesString;
		}
		else {
			if (_average) {
				auto images = QStringList();

				for (std::uint32_t i = 0; i < _dataset->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				_imageNames << imagesString;
			}
			else {
				for (std::uint32_t i = 0; i < _dataset->noImages(); i++) {
					_imageNames << QString("%1").arg(imageFileNames[i]);
				}
			}
		}
	}

	if (_dataset->imageCollectionType() == ImageCollectionType::Stack) {
		if (_average) {
			_dimensionNames << imageFileNames.join(", ");
		}
		else {
			_dimensionNames = imageFileNames;
		}
	}

	addLayer("ColorImage");
	addLayer("SelectionImage");
}

ImageDataset::~ImageDataset() = default;

QString ImageDataset::name() const
{
	return _name;
}

QSize ImageDataset::imageSize() const
{
	return _dataset->imageSize();
}

QStringList ImageDataset::imageNames() const
{
	return _imageNames;
}

QStringList ImageDataset::dimensionNames() const
{
	return _dimensionNames;
}

auto ImageDataset::currentImageId() const
{
	return _currentImageIndex;
}

void ImageDataset::setCurrentImageId(const std::int32_t& currentImageIndex)
{
	if (currentImageIndex == _currentImageIndex)
		return;

	if (currentImageIndex < 0)
		return;

	_currentImageIndex = currentImageIndex;

	qDebug() << _name << "set current image index" << _currentImageIndex;

	emit currentImageIndexChanged(_currentImageIndex);

	computeColorImage();
}

auto ImageDataset::currentDimensionIndex() const
{
	return _currentDimensionIndex;
}

void ImageDataset::setCurrentDimensionIndex(const std::int32_t& currentDimensionIndex)
{
	if (currentDimensionIndex == _currentDimensionIndex)
		return;

	if (currentDimensionIndex < 0)
		return;

	_currentDimensionIndex = currentDimensionIndex;

	qDebug() << _name << "set current dimension index" << _currentDimensionIndex;

	emit currentDimensionIndexChanged(_currentDimensionIndex);

	computeColorImage();
}

bool ImageDataset::average() const
{
	return _average;
}

void ImageDataset::setAverage(const bool& average)
{
	if (average == _average)
		return;

	_average = average;

	qDebug() << _name << "set average" << _average;

	emit averageChanged(_average);

	computeColorImage();
}

bool ImageDataset::canAverage() const
{
	return _dataset->imageCollectionType() == ImageCollectionType::Sequence;
}

float ImageDataset::selectionOpacity() const
{
	return _selectionOpacity;
}

void ImageDataset::setSelectionOpacity(const float& selectionOpacity)
{
	if (selectionOpacity == _selectionOpacity)
		return;

	_selectionOpacity = selectionOpacity;

	qDebug() << _name << "set selection opacity" << _selectionOpacity;

	emit selectionOpacityChanged(_selectionOpacity);
}

void ImageDataset::setSelectionChanged()
{
	emit selectionChanged();

	computeSelectionImage();
}

ImageCollectionType ImageDataset::imageCollectionType() const
{
	return _dataset->imageCollectionType();
}

QMenu* ImageDataset::contextMenu() const
{
	auto contextMenu = new QMenu();
	/*
	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		auto* createSubsetFromSelectionAction = new QAction("Create subset from selection");

		createSubsetFromSelectionAction->setEnabled(_imageViewerPlugin->noSelectedPixels() > 0);

		connect(createSubsetFromSelectionAction, &QAction::triggered, _imageViewerPlugin, &ImageViewerPlugin::createSubsetFromSelection);

		contextMenu->addSeparator();
		contextMenu->addAction(createSubsetFromSelectionAction);
	}
	*/

	return contextMenu;
}

void ImageDataset::activate()
{
	computeColorImage();
	computeSelectionImage();
}

std::vector<std::uint32_t> ImageDataset::selection() const
{
	return _dataset->indices();
}

std::uint32_t ImageDataset::noSelectedPixels() const
{
	return _dataset->noSelectedPixels();
}

bool ImageDataset::hasSelection() const
{
	return noSelectedPixels() > 0;
}

bool ImageDataset::isPixelSelectionAllowed() const
{
	return _dataset->imageCollectionType() == ImageCollectionType::Stack;
}

void ImageDataset::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
	_dataset->selectPixels(pixelCoordinates, selectionModifier);
}

void ImageDataset::createSubsetFromSelection()
{
	qDebug() << "Create subset from selection";

	_dataset->createSubset();
}

void ImageDataset::addLayer(const QString& name)
{
	try
	{
		if (_imageLayers.contains(name))
			throw std::exception(QString("%1 already exists").arg(name).toLatin1());

		_imageLayers.insert(name, new ImageLayer(this, name));

		emit layerAdded(name);
	}
	catch (const std::exception& e)
	{
		qDebug() << "Unable to add layer:" << e.what();
	}
}

void ImageDataset::removeLayer(const QString& name)
{
	try
	{
		if (!_imageLayers.contains(name))
			throw std::exception(QString("%1 does not exist").arg(name).toLatin1());

		_imageLayers.remove(name);

		emit layerRemoved(name);
	}
	catch (std::exception& e)
	{
		qDebug() << "Unable to remove layer:" << e.what();
	}
}

const ImageLayer* ImageDataset::imageLayerByName(const QString& name) const
{
	try {
		if (!_imageLayers.contains(name))
			throw std::exception(QString("%1 does not exist in %2").arg(name, _name).toLatin1());

		return _imageLayers[name];
	}
	catch (const std::exception& e)
	{
		throw std::exception(QString("Unable to retrieve layer by name: %1").arg(e.what()).toLatin1());
	}
}

ImageLayer* ImageDataset::imageLayerByName(const QString& name)
{
	const auto constThis = const_cast<const ImageDataset*>(this);
	return const_cast<ImageLayer*>(constThis->imageLayerByName(name));
}

const ImageLayers ImageDataset::imageLayers() const
{
	return _imageLayers;
}

void ImageDataset::computeColorImage()
{
	qDebug() << _name << "compute color image";

	switch (imageCollectionType()) {
		case ImageCollectionType::Sequence:
		{
			auto ids = std::vector<std::uint32_t>();

			if (hasSelection()) {
				const auto pointSelection = selection();
				ids = _average ? pointSelection : std::vector<std::uint32_t>({ pointSelection.front() });
			}
			else
			{
				if (_average) {
					ids.resize(_dataset->noImages());
					std::iota(ids.begin(), ids.end(), 0);
				}
				else
				{
					ids = std::vector<std::uint32_t>({ static_cast<std::uint32_t>(_currentImageIndex) });
				}
			}

			imageLayerByName("ColorImage")->setImage(*_dataset->sequenceImage(ids).get());
			break;
		}

		case ImageCollectionType::Stack:
		{
			imageLayerByName("ColorImage")->setImage(*_dataset->stackImage(_currentDimensionIndex).get());
			break;
		}

		default:
			break;
	}
}

void ImageDataset::computeSelectionImage()
{
	qDebug() << _name << "compute selection image";

	if (imageCollectionType() == ImageCollectionType::Stack) {
		imageLayerByName("SelectionImage")->setImage(*_dataset->selectionImage().get());
	}
}