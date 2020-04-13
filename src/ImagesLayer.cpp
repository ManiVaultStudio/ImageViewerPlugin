#include "ImagesLayer.h"
#include "ImagesDataset.h"

#include "ImageData/Images.h"

#include <QDebug>

ImagesLayer::ImagesLayer(ImagesDataset* imagesDataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(imagesDataset, LayerNode::Type::Images, id, name, flags),
	_currentImage(0),
	_average(),
	_image(),
	_images(imagesDataset)
{
}

int ImagesLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags ImagesLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
		case Column::Width:
		case Column::Height:
			break;
		
		case Column::WindowNormalized:
		case Column::LevelNormalized:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::ImageSize:
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
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return flags;
}

QVariant ImagesLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
			return _images->noImages(role);

		case Column::Width:
			return _images->width(role);

		case Column::Height:
			return _images->height(role);

		case Column::WindowNormalized:
			return _image.windowNormalized(role);

		case Column::LevelNormalized:
			return _image.levelNormalized(role);

		case Column::ImageSize:
			return _images->imageSize(role);

		case Column::NoPoints:
			return _images->noPoints(role);

		case Column::NoDimensions:
			return _images->noDimensions(role);

		case Column::ImageNames:
			return _images->imageNames(role);

		case Column::FilteredImageNames:
			return filteredImageNames(role);

		case Column::ImageIDs:
			return _images->imageIds(role);

		case Column::ImageFilePaths:
			return _images->imageFilePaths(role);

		case Column::CurrentImageId:
			return currentImageId(role);

		case Column::CurrentImageName:
			return currentImageName(role);

		case Column::CurrentImageFilePath:
			return currentImageFilePath(role);

		case Column::Average:
			return average(role);

		case Column::Selection:
			return _images->selection(role);

		case Column::SelectionSize:
			return _images->selectionSize(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList ImagesLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (index.column() < ult(Column::Start))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIndices{ index };

	switch (static_cast<Column>(index.column())) {
		case Column::NoImages:
		case Column::Width:
		case Column::Height:
			break;

		case Column::WindowNormalized:
			_image.setWindowNormalized(value.toFloat());
			break;

		case Column::LevelNormalized:
			_image.setLevelNormalized(value.toFloat());
			break;

		case Column::ImageSize:
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
			affectedIndices << index.siblingAtColumn(ult(Column::FilteredImageNames));
			break;

		case Column::Selection:
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return affectedIndices;
}

QVariant ImagesLayer::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize	= _images->selectionSize(Qt::EditRole).toInt();
	const auto imageNames		= _images->imageNames(Qt::EditRole).toStringList();

	auto filtered = QStringList();

	switch (_images->imageDataType(Qt::EditRole).toInt())
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : _images->selection().value<Indices>()) {
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
	const auto imageNames	= _images->imageNames(Qt::EditRole).toStringList();
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
	const auto imageFilePathString = _images->imageFilePaths(Qt::EditRole).toStringList()[_currentImage];

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
	switch (static_cast<ImageData::Type>(_images->type(Qt::EditRole).toInt()))
	{
		case ImageData::Type::Sequence:
		{
			auto selection = _images->selection().value<Indices>();

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