#include "Dataset.h"

#include <QDebug>

Dataset::Dataset(const QString& name, Images* images) : 
	QObject(),
	_name(name),
	_images(images),
	_imageNames(),
	_currentImageId(0),
	_dimensionNames(),
	_currentDimensionId(0),
	_average(false)
{
}

Dataset::~Dataset() = default;

QSize Dataset::imageSize() const
{
	return _images->imageSize();
}

auto Dataset::currentImageId() const
{
	return _currentImageId;
}

void Dataset::setCurrentImageId(const std::int32_t& currentImageIndex)
{
	if (currentImageIndex == _currentImageId)
		return;

	if (currentImageIndex < 0)
		return;

	_currentImageId = currentImageIndex;

	qDebug() << "Set current image" << _currentImageId;

	//TODO computeSelectionImage();
}

auto Dataset::currentDimensionIndex() const
{
	return _currentDimensionId;
}

void Dataset::setCurrentDimensionIndex(const std::int32_t& currentDimensionIndex)
{
	if (currentDimensionIndex == _currentDimensionId)
		return;

	if (currentDimensionIndex < 0)
		return;

	qDebug() << "Set current dimension index";

	_currentDimensionId = currentDimensionIndex;
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

	qDebug() << "Set average" << _average;

	//computeDisplayImage();
	//update();
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