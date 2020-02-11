#include "Dataset.h"

#include <QFileInfo>
#include <QDebug>

Dataset::Dataset(const QString& name, Images* images) : 
	QObject(),
	_name(name),
	_images(images),
	_imageNames(),
	_currentImageIndex(0),
	_dimensionNames(),
	_currentDimensionIndex(0),
	_average(false),
	_selectionOpacity(0.5f)
{
	auto imageFileNames = QStringList();

	for (const auto& imageFilePath : _images->imageFilePaths())
	{
		imageFileNames << QFileInfo(imageFilePath).fileName();
	}

	if (_images->imageCollectionType() == ImageCollectionType::Sequence) {
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

				for (std::uint32_t i = 0; i < _images->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				_imageNames << imagesString;
			}
			else {
				for (std::uint32_t i = 0; i < _images->noImages(); i++) {
					_imageNames << QString("%1").arg(imageFileNames[i]);
				}
			}
		}
	}

	if (_images->imageCollectionType() == ImageCollectionType::Stack) {
		if (_average) {
			_dimensionNames << imageFileNames.join(", ");
		}
		else {
			_dimensionNames = imageFileNames;
		}
	}
}

Dataset::~Dataset() = default;

QString Dataset::name() const
{
	return _name;
}

QSize Dataset::imageSize() const
{
	return _images->imageSize();
}

QStringList Dataset::imageNames() const
{
	return _imageNames;
}

QStringList Dataset::dimensionNames() const
{
	return _dimensionNames;
}

auto Dataset::currentImageId() const
{
	return _currentImageIndex;
}

void Dataset::setCurrentImageId(const std::int32_t& currentImageIndex)
{
	if (currentImageIndex == _currentImageIndex)
		return;

	if (currentImageIndex < 0)
		return;

	_currentImageIndex = currentImageIndex;

	qDebug() << _name << "set current image index" << _currentImageIndex;

	emit currentImageIndexChanged(_currentImageIndex);
}

auto Dataset::currentDimensionIndex() const
{
	return _currentDimensionIndex;
}

void Dataset::setCurrentDimensionIndex(const std::int32_t& currentDimensionIndex)
{
	if (currentDimensionIndex == _currentDimensionIndex)
		return;

	if (currentDimensionIndex < 0)
		return;

	_currentDimensionIndex = currentDimensionIndex;

	qDebug() << _name << "set current dimension index" << _currentDimensionIndex;

	emit currentDimensionIndexChanged(_currentDimensionIndex);
}

bool Dataset::average() const
{
	return _average;
}

void Dataset::setAverage(const bool& average)
{
	if (average == _average)
		return;

	_average = average;

	qDebug() << _name << "set average" << _average;

	emit averageChanged(_average);
}

bool Dataset::canAverage() const
{
	return _images->imageCollectionType() == ImageCollectionType::Sequence;
}

float Dataset::selectionOpacity() const
{
	return _selectionOpacity;
}

void Dataset::setSelectionOpacity(const float& selectionOpacity)
{
	if (selectionOpacity == _selectionOpacity)
		return;

	_selectionOpacity = selectionOpacity;

	qDebug() << _name << "set selection opacity" << _selectionOpacity;

	emit selectionOpacityChanged(_selectionOpacity);
}

ImageCollectionType Dataset::imageCollectionType() const
{
	return _images->imageCollectionType();
}

std::vector<std::uint32_t> Dataset::selection() const
{
	return _images->indices();
}

std::uint32_t Dataset::noSelectedPixels() const
{
	return _images->noSelectedPixels();
}

bool Dataset::hasSelection() const
{
	return noSelectedPixels() > 0;
}

bool Dataset::isPixelSelectionAllowed() const
{
	return _images->imageCollectionType() == ImageCollectionType::Stack;
}

void Dataset::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
	_images->selectPixels(pixelCoordinates, selectionModifier);
}

void Dataset::createSubsetFromSelection()
{
	qDebug() << "Create subset from selection";

	_images->createSubset();
}

/*
void ImageViewerPlugin::computeDisplayImage()
{
	if (_currentImages == nullptr)
		return;

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
					ids.resize(_currentImages->noImages());
					std::iota(ids.begin(), ids.end(), 0);
				}
				else
				{
					ids = std::vector<std::uint32_t>({ static_cast<std::uint32_t>(_currentImageId) });
				}
			}

			auto image = _currentImages->sequenceImage(ids);

			emit displayImageChanged(image);
			break;
		}

		case ImageCollectionType::Stack:
		{
			auto image = _currentImages->stackImage(_currentDimensionId);

			emit displayImageChanged(image);
			break;
		}

		default:
			break;
	}
}

void ImageViewerPlugin::computeSelectionImage()
{
	if (_currentImages == nullptr)
		return;

	qDebug() << "Compute selection image";

	if (imageCollectionType() == ImageCollectionType::Stack) {
		emit selectionImageChanged(_currentImages->selectionImage(), _currentImages->selectionBounds(true));
	}
	else {
		emit selectionImageChanged(std::make_shared<QImage>(), QRect());
	}
	
}
*/