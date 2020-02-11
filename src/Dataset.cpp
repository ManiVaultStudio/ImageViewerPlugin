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

/*
void ImageViewerPlugin::update()
{
	if (_currentImages == nullptr)
		return;

	auto imageFileNames = QStringList();

	foreach(const QString& imageFilePath, _currentImages->imageFilePaths())
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

				for (std::uint32_t i = 0; i < _currentImages->noImages(); i++) {
					images << QString("%1").arg(imageFileNames[i]);
				}

				const auto imagesString = images.join(", ");

				imageNames << imagesString;
			}
			else {
				for (std::uint32_t i = 0; i < _currentImages->noImages(); i++) {
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