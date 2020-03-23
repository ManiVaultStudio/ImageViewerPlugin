#include "ImagesSettings.h"

#include "ImageData/Images.h"

#include <QDebug>

ImagesSettings::ImagesSettings(QObject* parent, Dataset* dataset) :
	Settings(parent, _dataset),
	_currentImage(0),
	_average()
{
}

Qt::ItemFlags ImagesSettings::itemFlags(const Layer::Column& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Layer::Column>(column)) {
		case Layer::Column::NoImages:
		case Layer::Column::Width:
		case Layer::Column::Height:
		case Layer::Column::Size:
		case Layer::Column::NoPoints:
		case Layer::Column::NoDimensions:
		case Layer::Column::ImageNames:
		case Layer::Column::FilteredImageNames:
		case Layer::Column::ImageIDs:
		case Layer::Column::ImageFilePaths:
		case Layer::Column::CurrentImageId:
			flags |= Qt::ItemIsEditable;
			break;

		case Layer::Column::CurrentImageName:
		case Layer::Column::CurrentImageFilePath:
			break;

		case Layer::Column::Average:
			flags |= Qt::ItemIsEditable;
			break;

		case Layer::Column::Selection:

			break;
		default:
			break;
	}

	return flags;
}

QVariant ImagesSettings::data(const Layer::Column& column, int role) const
{
	switch (static_cast<Layer::Column>(column)) {
		case Layer::Column::NoImages:
			_dataset->noImages(role);

		case Layer::Column::Width:
			_dataset->width(role);

		case Layer::Column::Height:
			_dataset->height(role);

		case Layer::Column::Size:
			_dataset->size(role);

		case Layer::Column::NoPoints:
			_dataset->noPoints(role);

		case Layer::Column::NoDimensions:
			_dataset->noDimensions(role);

		case Layer::Column::ImageNames:
			_dataset->imageNames(role);

		case Layer::Column::FilteredImageNames:
			return filteredImageNames(role);

		case Layer::Column::ImageIDs:
			_dataset->imageIds(role);

		case Layer::Column::ImageFilePaths:
			_dataset->imageFilePaths(role);

		case Layer::Column::CurrentImageId:
			return currentImageId(role);

		case Layer::Column::CurrentImageName:
			return currentImageName(role);

		case Layer::Column::CurrentImageFilePath:
			return currentImageFilePath(role);

		case Layer::Column::Average:
			return average(role);

		case Layer::Column::Selection:
			_dataset->selection(role);

		default:
			break;
	}

	return QVariant();
}

void ImagesSettings::setData(const Layer::Column& column, const QVariant& value, const int& role)
{
	switch (static_cast<Layer::Column>(column)) {
		case Layer::Column::NoImages:
		case Layer::Column::Width:
		case Layer::Column::Height:
		case Layer::Column::Size:
		case Layer::Column::NoPoints:
		case Layer::Column::NoDimensions:
		case Layer::Column::ImageNames:
		case Layer::Column::FilteredImageNames:
		case Layer::Column::ImageIDs:
		case Layer::Column::ImageFilePaths:
			break;

		case Layer::Column::CurrentImageId:
			setCurrentImageId(value.toInt());
			break;

		case Layer::Column::CurrentImageName:
		case Layer::Column::CurrentImageFilePath:
			break;

		case Layer::Column::Average:
			setAverage(value.toBool());
			break;

		case Layer::Column::Selection:
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