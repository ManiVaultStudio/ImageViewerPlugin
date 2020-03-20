#include "ImagesDataset.h"
#include "ImageViewerPlugin.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QFont>
#include <QFileInfo>
#include <QDebug>

ImagesDataset::ImagesDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name) :
	Dataset(imageViewerPlugin, name, Type::Images),
	_size(),
	_noPoints(0),
	_noDimensions(0),
	_imageNames(),
	_imageFilePaths(),
	_average(),
	_pointsName(),
	_selection(Indices({ 0 }))
{
}

void ImagesDataset::init()
{
	auto images = _imageViewerPlugin->requestData<Images>(_name);

	setSize(images.imageSize());
	setNoPoints(images.points()->getNumPoints());
	setNoDimensions(images.points()->getNumDimensions());
	setCurrentImage(0);
	setAverage(false);

	auto imageFilePaths = QStringList();

	for (const auto& imageFilePath : images.imageFilePaths()) {
		imageFilePaths << imageFilePath;
	}

	switch (images.type())
	{
		case ImageData::Type::Sequence:
		{
			auto imageNames = QStringList();

			for (const auto& imageFilePath : images.imageFilePaths()) {
				imageNames << QFileInfo(imageFilePath).fileName();
			}

			setImageNames(imageNames);
			break;
		}

		case ImageData::Type::Stack:
		{
			auto dimensionNames = QStringList();

			for (const auto& dimensionName : images.dimensionNames()) {
				dimensionNames << dimensionName;
			}

			setImageNames(dimensionNames);
			break;
		}

		default:
			break;
	}

	setImageFilePaths(imageFilePaths);
	setPointsName(images.points()->getDataName());
}

int ImagesDataset::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent)

		return 0;
}

QVariant ImagesDataset::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

Qt::ItemFlags ImagesDataset::flags(const QModelIndex &index) const
{
	return 0;
}

QVariant ImagesDataset::data(const int& row, const int& column, int role) const
{
	return QVariant();
}

void ImagesDataset::setData(const int& row, const int& column, const QVariant& value)
{
}

QVariant ImagesDataset::noImages(const int& role /*= Qt::DisplayRole*/) const
{
	const auto count = _imageFilePaths.size();

	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(count);

		case Qt::EditRole:
			return count;

		case Qt::ToolTipRole:
			return QString("Number of images: %1").arg(QString::number(count));

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesDataset::width(const int& role /*= Qt::DisplayRole*/) const
{
	const auto widthString = QString::number(_size.width());

	switch (role)
	{
		case Qt::DisplayRole:
			return widthString;

		case Qt::EditRole:
			return _size.width();

		case Qt::ToolTipRole:
			return QString("Image width: %1 pixels").arg(widthString);

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesDataset::height(const int& role /*= Qt::DisplayRole*/) const
{
	const auto heightString = QString::number(_size.height());

	switch (role)
	{
		case Qt::DisplayRole:
			return heightString;

		case Qt::EditRole:
			return _size.height();

		case Qt::ToolTipRole:
			return QString("Image height: %1 pixels").arg(heightString);

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesDataset::size(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString("%1x%2").arg(QString::number(_size.width()), QString::number(_size.height()));

		case Qt::EditRole:
			return _size;

		case Qt::ToolTipRole:
			return QString("Image size: %1x%2").arg(QString::number(_size.width()), QString::number(_size.height()));

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setSize(const QSize& size)
{
	_size = size;
}

QVariant ImagesDataset::noPoints(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_noPoints);

		case Qt::EditRole:
			return _noPoints;

		case Qt::ToolTipRole:
			return QString("Number of data points: %1").arg(QString::number(_noPoints));

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setNoPoints(const std::uint32_t& noPoints)
{
	_noPoints = noPoints;
}

QVariant ImagesDataset::noDimensions(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return QString::number(_noDimensions);

		case Qt::EditRole:
			return _noDimensions;

		case Qt::ToolTipRole:
			return QString("Number of data dimensions: %1").arg(QString::number(_noDimensions));

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setNoDimensions(const std::uint32_t& noDimensions)
{
	_noDimensions = noDimensions;
}

QVariant ImagesDataset::imageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageNamesString = ImagesDataset::displayStringList(_imageNames);

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return _imageNames;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesDataset::filteredImageNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize = _selection.size();

	auto imageNames = QStringList();

	switch (type(Qt::EditRole).toInt())
	{
		case ImageData::Type::Sequence:
		{
			if (selectionSize > 0) {
				for (auto id : _selection) {
					imageNames << _imageNames[id];
				}
			}
			else {
				imageNames << _imageNames;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			imageNames = _imageNames;
			break;
		}

		default:
			break;
	}

	auto imageNamesString = QString();

	if (_average) {
		imageNamesString = ImagesDataset::displayStringList(imageNames);
	}
	else {
		imageNamesString = _imageNames.isEmpty() ? "" : _imageNames[_currentImage];
	}

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return imageNames;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}

	return QVariant();
}

QVariant ImagesDataset::imageIds(const int& role /*= Qt::DisplayRole*/) const
{
	const auto selectionSize = _selection.size();

	auto ids = Indices();

	/*
	switch (_type)
	{
		case ImageData::Type::Sequence:
		{
			if (_average) {
				if (selectionSize > 0) {
					for (auto id : _selection) {
						ids << id;
					}
				}
				else {
					ids.resize(_imageNames.size());
					std::iota(ids.begin(), ids.end(), 0);
				}
			}
			else {
				ids << _currentImage;
			}

			break;
		}

		case ImageData::Type::Stack:
		{
			if (_average) {
				ids.resize(_imageNames.size());
				std::iota(ids.begin(), ids.end(), 0);
			}
			else {
				ids << _currentImage;
			}

			break;
		}

		default:
			break;
	}
	*/
	auto imageIdsString = QString();

	if (ids.size() == 1)
		imageIdsString = QString::number(ids.first());

	if (ids.size() == 2)
		imageIdsString = QString("[%1, %2]").arg(QString::number(ids.first()), QString::number(ids.last()));

	if (ids.size() > 2)
		imageIdsString = QString("[%1, ..., %2]").arg(QString::number(ids.first()), QString::number(ids.last()));

	switch (role)
	{
		case Qt::DisplayRole:
			return imageIdsString;

		case Qt::EditRole:
			return QVariant::fromValue(ids);

		case Qt::ToolTipRole:
			return QString("Image ID's: %1").arg(imageIdsString);

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setImageNames(const QStringList& imageNames)
{
	_imageNames = imageNames;
}

QVariant ImagesDataset::imageFilePaths(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageFilePathsString = QString("[%1]").arg(_imageFilePaths.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return imageFilePathsString;

		case Qt::EditRole:
			return _imageFilePaths;

		case Qt::ToolTipRole:
			return QString("Image file paths: %1").arg(imageFilePathsString);

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setImageFilePaths(const QStringList& imageFilePaths)
{
	_imageFilePaths = imageFilePaths;
}

QVariant ImagesDataset::currentImage(const int& role /*= Qt::DisplayRole*/) const
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

void ImagesDataset::setCurrentImage(const std::uint32_t& currentImage)
{
	/*
	switch (_type)
	{
		case ImageData::Type::Sequence:
		{
			if (_selection.isEmpty())
				_currentImage = currentImage;
			else
				_currentImage = _selection[currentImage];

			break;
		}

		case ImageData::Type::Stack:
			_currentImage = currentImage;
			break;

		default:
			break;
	}
	*/
}

QVariant ImagesDataset::currentImageName(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageName = _imageNames.isEmpty() ? "" : _imageNames[_currentImage];

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

void ImagesDataset::setAverage(const bool& average)
{
	_average = average;
}

QVariant ImagesDataset::average(const int& role /*= Qt::DisplayRole*/) const
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

QVariant ImagesDataset::currentImageFilePath(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageFilePathString = _imageFilePaths[_currentImage];

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

QVariant ImagesDataset::pointsName(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _pointsName;

		case Qt::ToolTipRole:
			return QString("Points dataset name: %1").arg(_pointsName);

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setPointsName(const QString& pointsName)
{
	_pointsName = pointsName;
}

QVariant ImagesDataset::selection(const int& role /*= Qt::DisplayRole*/) const
{
	auto selection = QStringList();

	if (_selection.size() <= 2) {
		for (const auto& id : _selection)
			selection << QString::number(id);
	}
	else {
		selection << QString::number(_selection.first());
		selection << "...";
		selection << QString::number(_selection.last());
	}

	const auto selectionString = QString("[%1]").arg(selection.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionString;

		case Qt::EditRole:
			return QVariant::fromValue(_selection);

		case Qt::ToolTipRole:
			return QString("Selection: %1").arg(selectionString);

		default:
			break;
	}

	return QVariant();
}

void ImagesDataset::setSelection(const Indices& selection)
{
	/*
	_selection = selection;

	if (_type == ImageData::Type::Sequence)
		_currentImage = selection.isEmpty() ? 0 : selection.first();
	*/
}

QVariant ImagesDataset::selectionSize(const int& role /*= Qt::DisplayRole*/) const
{
	/*
	const auto selectionSizeString = QString::number(_selection.size());

	switch (role)
	{
		case Qt::DisplayRole:
			return selectionSizeString;

		case Qt::EditRole:
			return _selection.size();

		case Qt::ToolTipRole:
		{
			switch (_type)
			{
				case ImageData::Type::Sequence:
					return QString("No. selected images: %1").arg(selectionSizeString);

				case ImageData::Type::Stack:
					return QString("No. selected pixels: %1").arg(selectionSizeString);

				default:
					break;
			}
		}

		default:
			break;
	}
	*/

	return QVariant();
}

QString ImagesDataset::displayStringList(const QStringList& stringList)
{
	const auto noStrings = stringList.size();

	if (noStrings == 1)
		return QString("%1").arg(stringList.first());

	if (noStrings == 2)
		return QString("[%1, %2]").arg(stringList.first(), stringList.last());

	if (noStrings > 2)
		return QString("[%1, ..., %2]").arg(stringList.first(), stringList.last());

	return QString("[]");
}