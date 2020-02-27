#include "ImageViewerPlugin.h"
#include "ImageDatasetsModel.h"
#include "ImageDataset.h"

#include <QDebug>

ImageDatasetsModel::ImageDatasetsModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractTableModel(imageViewerPlugin),
	_imageDatasets(),
	_currentDatasetName(),
	_selectionModel(this)
{
}

ImageDatasetsModel::~ImageDatasetsModel() = default;

void ImageDatasetsModel::add(const QString& name, const QString& type, const std::uint32_t& noImages, const QSize& size, const std::uint32_t& noPoints, const std::uint32_t& noDimensions)
{
	insertRows(0, 1, QModelIndex());

	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::Name), QModelIndex()), name);
	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::Type), QModelIndex()), type);
	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::NoImages), QModelIndex()), noImages);
	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::Size), QModelIndex()), size);
	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::NoPoints), QModelIndex()), noPoints);
	setData(index(0, static_cast<int>(ImageDatasetsModel::Columns::NoDimensions), QModelIndex()), noDimensions);
}

int ImageDatasetsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return _imageDatasets.size();
}

int ImageDatasetsModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 6;
}

QVariant ImageDatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _imageDatasets.size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole) {
		auto imageDataset = _imageDatasets.at(index.row());

		if (index.column() == static_cast<int>(Columns::Name))
			return imageDataset._name;
		else if (index.column() == static_cast<int>(Columns::Type))
			return imageDataset._type;
		else if (index.column() == static_cast<int>(Columns::NoImages))
			return QString::number(imageDataset._noImages);
		else if (index.column() == static_cast<int>(Columns::Size))
			return QString("%1x%2").arg(QString::number(imageDataset._size.width()), QString::number(imageDataset._size.height()));
		else if (index.column() == static_cast<int>(Columns::NoPoints))
			return QString::number(imageDataset._noPoints);
		else if (index.column() == static_cast<int>(Columns::NoDimensions))
			return QString::number(imageDataset._noDimensions);
	}

	return QVariant();
}

QVariant ImageDatasetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case static_cast<int>(Columns::Name) :
				return tr("Name");

			case static_cast<int>(Columns::Type) :
				return tr("Type");

			case static_cast<int>(Columns::NoImages) :
				return tr("No. images");

			case static_cast<int>(Columns::Size) :
				return tr("Size");

			case static_cast<int>(Columns::NoPoints) :
				return tr("No. Points");

			case static_cast<int>(Columns::NoDimensions) :
				return tr("No. Dimensions");

			default:
				return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags ImageDatasetsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool ImageDatasetsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto imageDataset = _imageDatasets.value(row);

		if (index.column() == static_cast<int>(Columns::Name))
			imageDataset._name = value.toString();
		else if (index.column() == static_cast<int>(Columns::Type))
			imageDataset._type = value.toString();
		else if (index.column() == static_cast<int>(Columns::NoImages))
			imageDataset._noImages = value.toInt();
		else if (index.column() == static_cast<int>(Columns::Size))
			imageDataset._size = value.toSize();
		else if (index.column() == static_cast<int>(Columns::NoPoints))
			imageDataset._noPoints = value.toInt();
		else if (index.column() == static_cast<int>(Columns::NoDimensions))
			imageDataset._noDimensions = value.toInt();
		else
			return false;

		_imageDatasets.replace(row, imageDataset);

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

bool ImageDatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_imageDatasets.insert(position, ImageDataset());
	}

	endInsertRows();

	return true;
}

bool ImageDatasetsModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_imageDatasets.removeAt(position);
	}

	endRemoveRows();

	return true;
}

QList<ImageDatasetsModel::ImageDataset> ImageDatasetsModel::imageDatasets()
{
	return _imageDatasets;
}