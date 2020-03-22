#include "ImagesSettings.h"

#include "ImageData/Images.h"

#include <QDebug>

ImagesSettings::ImagesSettings(Dataset* _dataset) :
	Settings(_dataset),
	_currentImage(0),
	_average()
{
}

Qt::ItemFlags ImagesSettings::itemFlags(const LayerColumn& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<LayerColumn>(column)) {
		case LayerColumn::NoImages:
		case LayerColumn::Width:
		case LayerColumn::Height:
		case LayerColumn::Size:
		case LayerColumn::NoPoints:
		case LayerColumn::NoDimensions:
		case LayerColumn::ImageNames:
		case LayerColumn::FilteredImageNames:
		case LayerColumn::ImageIDs:
		case LayerColumn::ImageFilePaths:
		case LayerColumn::CurrentImageId:
			flags |= Qt::ItemIsEditable;
			break;

		case LayerColumn::CurrentImageName:
		case LayerColumn::CurrentImageFilePath:
			break;

		case LayerColumn::Average:
			flags |= Qt::ItemIsEditable;
			break;

		case LayerColumn::Selection:

			break;
		default:
			break;
	}

	return flags;
}

QVariant ImagesSettings::data(const LayerColumn& column, int role) const
{
	switch (static_cast<LayerColumn>(column)) {
		case LayerColumn::NoImages:
			_dataset->noImages(role);

		case LayerColumn::Width:
			_dataset->width(role);

		case LayerColumn::Height:
			_dataset->height(role);

		case LayerColumn::Size:
			_dataset->size(role);

		case LayerColumn::NoPoints:
			_dataset->noPoints(role);

		case LayerColumn::NoDimensions:
			_dataset->noDimensions(role);

		case LayerColumn::ImageNames:
			_dataset->imageNames(role);

		case LayerColumn::FilteredImageNames:
			return filteredImageNames(role);

		case LayerColumn::ImageIDs:
			_dataset->imageIds(role);

		case LayerColumn::ImageFilePaths:
			_dataset->imageFilePaths(role);

		case LayerColumn::CurrentImageId:
			return currentImageId(role);

		case LayerColumn::CurrentImageName:
			return currentImageName(role);

		case LayerColumn::CurrentImageFilePath:
			return currentImageFilePath(role);

		case LayerColumn::Average:
			return average(role);

		case LayerColumn::Selection:
			_dataset->selection(role);

		default:
			break;
	}

	return QVariant();
}

void ImagesSettings::setData(const LayerColumn& column, const QVariant& value, const int& role)
{
	switch (static_cast<LayerColumn>(column)) {
		case LayerColumn::NoImages:
		case LayerColumn::Width:
		case LayerColumn::Height:
		case LayerColumn::Size:
		case LayerColumn::NoPoints:
		case LayerColumn::NoDimensions:
		case LayerColumn::ImageNames:
		case LayerColumn::FilteredImageNames:
		case LayerColumn::ImageIDs:
		case LayerColumn::ImageFilePaths:
			break;

		case LayerColumn::CurrentImageId:
			setCurrentImageId(value.toInt());
			break;

		case LayerColumn::CurrentImageName:
		case LayerColumn::CurrentImageFilePath:
			break;

		case LayerColumn::Average:
			setAverage(value.toBool());
			break;

		case LayerColumn::Selection:
			break;

		default:
			break;
	}
}

QVariant ImagesSettings::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
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

QVariant ImagesSettings::currentImageId(const int& role /*= Qt::DisplayRole*/) const
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

QVariant ImagesSettings::currentImageName(const int& role /*= Qt::DisplayRole*/) const
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

QVariant ImagesSettings::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
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

void ImagesSettings::setCurrentImageId(const std::uint32_t& currentImage)
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

QVariant ImagesSettings::average(const int& role /*= Qt::DisplayRole*/) const
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

void ImagesSettings::setAverage(const bool& average)
{
	_average = average;
}