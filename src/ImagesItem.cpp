#include "ImagesItem.h"
#include "LayerItem.h"
#include "ImagesDataset.h"

#include "ImageData/Images.h"

#include <QDebug>

ImagesItem::ImagesItem(LayerItem* layeritem, ImagesDataset* imagesDataset) :
	Item(layeritem),
	_currentImage(0),
	_average(),
	_dataset(imagesDataset)
{
}

int ImagesItem::columnCount() const
{
	return 0;
}

QVariant ImagesItem::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	return QVariant();
}

Qt::ItemFlags ImagesItem::flags(const int& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(column)) {
		case Column::NoImages:
		case Column::Width:
		case Column::Height:
		case Column::Size:
		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::ImageNames:
		case Column::FilteredImageNames:
		case Column::ImageIDs:
		case Column::ImageFilePaths:
		case Column::CurrentImageId:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::CurrentImageName:
		case Column::CurrentImageFilePath:
			break;

		case Column::Average:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Selection:
			break;

		default:
			break;
	}

	return flags;
}

QVariant ImagesItem::data(const int& column, const int& role) const
{
	switch (static_cast<Column>(column)) {
		case Column::NoImages:
			_dataset->noImages(role);

		case Column::Width:
			_dataset->width(role);

		case Column::Height:
			_dataset->height(role);

		case Column::Size:
			_dataset->size(role);

		case Column::NoPoints:
			_dataset->noPoints(role);

		case Column::NoDimensions:
			_dataset->noDimensions(role);

		case Column::ImageNames:
			_dataset->imageNames(role);

		case Column::FilteredImageNames:
			return filteredImageNames(role);

		case Column::ImageIDs:
			_dataset->imageIds(role);

		case Column::ImageFilePaths:
			_dataset->imageFilePaths(role);

		case Column::CurrentImageId:
			return currentImageId(role);

		case Column::CurrentImageName:
			return currentImageName(role);

		case Column::CurrentImageFilePath:
			return currentImageFilePath(role);

		case Column::Average:
			return average(role);

		case Column::Selection:
			_dataset->selection(role);

		default:
			break;
	}

	return QVariant();
}

void ImagesItem::setData(const int& column, const QVariant& value, const int& role)
{
	switch (static_cast<Column>(column)) {
		case Column::NoImages:
		case Column::Width:
		case Column::Height:
		case Column::Size:
		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::ImageNames:
		case Column::FilteredImageNames:
		case Column::ImageIDs:
		case Column::ImageFilePaths:
			break;

		case Column::CurrentImageId:
			setCurrentImageId(value.toInt());
			break;

		case Column::CurrentImageName:
		case Column::CurrentImageFilePath:
			break;

		case Column::Average:
			setAverage(value.toBool());
			break;

		case Column::Selection:
			break;

		default:
			break;
	}
}

QVariant ImagesItem::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize	= _dataset->selectionSize(Qt::EditRole).toInt();
	const auto imageNames		= _dataset->imageNames(Qt::EditRole).toStringList();

	auto filtered = QStringList();

	switch (_dataset->type(Qt::EditRole).toInt())
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : _dataset->selection().value<Indices>()) {
					filtered << imageNames[id];
				}
			}
			else {
				filtered << imageNames;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			filtered = imageNames;
			break;
		}

		default:
			break;
	}

	auto imageNamesString = QString();

	if (_average) {
		imageNamesString = ImagesDataset::displayStringList(filtered);
	}
	else {
		imageNamesString = imageNames.isEmpty() ? "" : imageNames[_currentImage];
	}

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return filtered;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}
	
	return QVariant();
}

QVariant ImagesItem::currentImageId(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_currentImage);

		case Qt::EditRole:
			return _currentImage;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(QString::number(_currentImage));
	}

	return QVariant();
}

QVariant ImagesItem::currentImageName(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageNames	= _dataset->imageNames(Qt::EditRole).toStringList();
	const auto imageName	= imageNames.isEmpty() ? "" : imageNames[_currentImage];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return imageName;

		case Qt::ToolTipRole:
			return QString("Current image: %1").arg(imageName);
	}

	return QVariant();
}

QVariant ImagesItem::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageFilePathString = _dataset->imageFilePaths(Qt::EditRole).toStringList()[_currentImage];

	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return imageFilePathString;

		case Qt::ToolTipRole:
			return QString("Image file path: %1").arg(imageFilePathString);

		default:
			break;
	}

	return QVariant();
}

void ImagesItem::setCurrentImageId(const std::uint32_t& currentImage)
{
	switch (static_cast<ImageData::Type>(_dataset->type(Qt::EditRole).toInt()))
	{
		case ImageData::Type::Sequence:
		{
			auto selection = _dataset->selection().value<Indices>();

			if (selection.isEmpty())
				_currentImage = currentImage;
			else
				_currentImage = selection[currentImage];

			break;
		}

		case ImageData::Type::Stack:
			_currentImage = currentImage;
			break;

		default:
			break;
	}
}

QVariant ImagesItem::average(const int& role /*= Qt::DisplayRole*/) const
{
	const auto averageString = _average ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return averageString;

		case Qt::EditRole:
			return _average;

		case Qt::ToolTipRole:
			return QString("Average images: %1").arg(averageString);

		default:
			break;
	}

	return QVariant();
}

void ImagesItem::setAverage(const bool& average)
{
	_average = average;
}