#include "Dataset.h"

#include <QFileInfo>
#include <QMenu>
#include <QDebug>

Dataset::Dataset(const QString& name, Images* images) : 
	QObject(),
	_name(name),
	_dataset(images),
	_imageNames(),
	_currentImageIndex(0),
	_dimensionNames(),
	_currentDimensionIndex(0),
	_average(false),
	_selectionOpacity(0.5f),
	_layers()
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
}

Dataset::~Dataset() = default;

QString Dataset::name() const
{
	return _name;
}

QSize Dataset::imageSize() const
{
	return _dataset->imageSize();
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

	computeColorImage();
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

	computeColorImage();
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

	computeColorImage();
}

bool Dataset::canAverage() const
{
	return _dataset->imageCollectionType() == ImageCollectionType::Sequence;
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

void Dataset::setSelectionChanged()
{
	emit selectionChanged();

	computeSelectionImage();
}

ImageCollectionType Dataset::imageCollectionType() const
{
	return _dataset->imageCollectionType();
}

QMenu* Dataset::contextMenu() const
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

std::vector<std::uint32_t> Dataset::selection() const
{
	return _dataset->indices();
}

std::uint32_t Dataset::noSelectedPixels() const
{
	return _dataset->noSelectedPixels();
}

bool Dataset::hasSelection() const
{
	return noSelectedPixels() > 0;
}

bool Dataset::isPixelSelectionAllowed() const
{
	return _dataset->imageCollectionType() == ImageCollectionType::Stack;
}

void Dataset::selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier)
{
	_dataset->selectPixels(pixelCoordinates, selectionModifier);
}

void Dataset::createSubsetFromSelection()
{
	qDebug() << "Create subset from selection";

	_dataset->createSubset();
}

void Dataset::computeColorImage()
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

			emit colorImageChanged(QSharedPointer<QImage>::create(*_dataset->sequenceImage(ids).get()));
			break;
		}

		case ImageCollectionType::Stack:
		{
			emit colorImageChanged(QSharedPointer<QImage>::create(*_dataset->stackImage(_currentDimensionIndex).get()));
			break;
		}

		default:
			break;
	}
}

void Dataset::computeSelectionImage()
{
	qDebug() << _name << "compute selection image";

	if (imageCollectionType() == ImageCollectionType::Stack) {
		emit selectionImageChanged(QSharedPointer<QImage>::create(*_dataset->selectionImage().get()), _dataset->selectionBounds(true));
	}
	else {
		emit selectionImageChanged(QSharedPointer<QImage>::create(), QRect());
	}
}