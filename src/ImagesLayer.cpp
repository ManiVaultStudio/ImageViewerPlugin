#include "ImagesLayer.h"

#include "ImageData/Images.h"

#include <QDebug>

ImagesLayer::ImagesLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags) :
	Layer(dataset, Type::Images, id, name, flags),
	_currentImage(0),
	_average()
{
}

Qt::ItemFlags ImagesLayer::itemFlags(const int& column) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::itemFlags(column);

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

QVariant ImagesLayer::data(const int& column, int role) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::data(column, role);

	switch (static_cast<Column>(column)) {
		case Column::NoImages:
			imagesDataset()->noImages(role);

		case Column::Width:
			imagesDataset()->width(role);

		case Column::Height:
			imagesDataset()->height(role);

		case Column::Size:
			imagesDataset()->size(role);

		case Column::NoPoints:
			imagesDataset()->noPoints(role);

		case Column::NoDimensions:
			imagesDataset()->noDimensions(role);

		case Column::ImageNames:
			imagesDataset()->imageNames(role);

		case Column::FilteredImageNames:
			return filteredImageNames(role);

		case Column::ImageIDs:
			imagesDataset()->imageIds(role);

		case Column::ImageFilePaths:
			imagesDataset()->imageFilePaths(role);

		case Column::CurrentImageId:
			return currentImageId(role);

		case Column::CurrentImageName:
			return currentImageName(role);

		case Column::CurrentImageFilePath:
			return currentImageFilePath(role);

		case Column::Average:
			return average(role);

		case Column::Selection:
			imagesDataset()->selection(role);

		default:
			break;
	}

	return QVariant();
}

void ImagesLayer::setData(const int& column, const QVariant& value, const int& role)
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::setData(column, value, role);

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

QVariant ImagesLayer::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize	= imagesDataset()->selectionSize(Qt::EditRole).toInt();
	const auto imageNames		= imagesDataset()->imageNames(Qt::EditRole).toStringList();

	auto filtered = QStringList();

	switch (imagesDataset()->type(Qt::EditRole).toInt())
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : imagesDataset()->selection().value<Indices>()) {
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

QVariant ImagesLayer::currentImageId(const int& role /*= Qt::DisplayRole*/) const
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

QVariant ImagesLayer::currentImageName(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageNames	= imagesDataset()->imageNames(Qt::EditRole).toStringList();
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

QVariant ImagesLayer::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageFilePathString = imagesDataset()->imageFilePaths(Qt::EditRole).toStringList()[_currentImage];

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

void ImagesLayer::setCurrentImageId(const std::uint32_t& currentImage)
{
	switch (static_cast<ImageData::Type>(imagesDataset()->type(Qt::EditRole).toInt()))
	{
		case ImageData::Type::Sequence:
		{
			auto selection = imagesDataset()->selection().value<Indices>();

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

QVariant ImagesLayer::average(const int& role /*= Qt::DisplayRole*/) const
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

void ImagesLayer::setAverage(const bool& average)
{
	_average = average;
}