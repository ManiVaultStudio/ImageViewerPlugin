#include "ImagesDataset.h"

#include <QDebug>

ImagesDataset::ImagesDataset() : 
	QObject(),
	_images(),
	_average(false)
{
}

ImagesDataset::~ImagesDataset() = default;

bool ImagesDataset::average() const
{
	return _average;
}

void ImagesDataset::setAverage(const bool& average)
{
	if (average == _average)
		return;

	_average = average;

	qDebug() << "Set average" << _average;

	//computeDisplayImage();
	//update();
}

std::vector<std::uint32_t> ImagesDataset::selection() const
{
	return _images->indices();
}

std::uint32_t ImagesDataset::noSelectedPixels() const
{
	return _images->noSelectedPixels();
}

bool ImagesDataset::hasSelection() const
{
	return noSelectedPixels() > 0;
}

bool ImagesDataset::isPixelSelectionAllowed() const
{
	return _images->imageCollectionType() == ImageCollectionType::Stack;
}

void ImagesDataset::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
	_images->selectPixels(pixelCoordinates, selectionModifier);
}

void ImagesDataset::createSubsetFromSelection()
{
	qDebug() << "Create subset from selection";

	_images->createSubset();
}