#include "DatasetsModel.h"
#include "MainModel.h"

#include <QDebug>

DatasetsModel::DatasetsModel(MainModel* mainModel) :
	QAbstractListModel(mainModel),
	_currentDatasetName()
{
}

DatasetsModel::~DatasetsModel() = default;

int DatasetsModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return _mainModel->imageDatasets().size();
}

int DatasetsModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 11;
}

QVariant DatasetsModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= columnCount(index) || index.row() < 0)
		return QVariant();

	auto imageDataset = _mainModel->imageDatasets().at(index.row());

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case (static_cast<int>(Columns::Name)) :
				return imageDataset._name;

			case (static_cast<int>(Columns::Type)):
				return imageDataset._type;

			case (static_cast<int>(Columns::NoImages)):
				return QString::number(imageDataset._noImages);

			case (static_cast<int>(Columns::Size)):
				return QString("%1x%2").arg(QString::number(imageDataset._size.width()), QString::number(imageDataset._size.height()));

			case (static_cast<int>(Columns::NoPoints)):
				return QString::number(imageDataset._noPoints);

			case (static_cast<int>(Columns::NoDimensions)):
				return QString::number(imageDataset._noDimensions);

			case (static_cast<int>(Columns::ImageID)):
				return QString::number(imageDataset._imageID);

			case (static_cast<int>(Columns::DimensionID)):
				return QString::number(imageDataset._dimensionID);

			case (static_cast<int>(Columns::ImageNames)):
				return QString("[%1]").arg(imageDataset._imageNames.join(", "));

			case (static_cast<int>(Columns::DimensionNames)):
				return QString("[%1]").arg(imageDataset._dimensionNames.join(", "));

			case (static_cast<int>(Columns::AverageImages)):
				return imageDataset._averageImages ? "true" : "false";

			default:
				break;
		}
	}

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				return imageDataset._name;

			case (static_cast<int>(Columns::Type)):
				return imageDataset._type;

			case (static_cast<int>(Columns::NoImages)):
				return QString::number(imageDataset._noImages);

			case (static_cast<int>(Columns::Size)):
				return imageDataset._size;

			case (static_cast<int>(Columns::NoPoints)):
				return imageDataset._noPoints;

			case (static_cast<int>(Columns::NoDimensions)):
				return imageDataset._noDimensions;

			case (static_cast<int>(Columns::ImageID)):
				return imageDataset._imageID;

			case (static_cast<int>(Columns::DimensionID)):
				return imageDataset._dimensionID;

			case (static_cast<int>(Columns::ImageNames)):
				return imageDataset._imageNames;

			case (static_cast<int>(Columns::DimensionNames)):
				return imageDataset._dimensionNames;

			case (static_cast<int>(Columns::AverageImages)):
				return imageDataset._averageImages;

			default:
				break;
		}
	}

	return QVariant();
}

QVariant DatasetsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case (static_cast<int>(Columns::Name)):
				return "Name";

			case (static_cast<int>(Columns::Type)):
				return "Type";

			case (static_cast<int>(Columns::NoImages)):
				return "No. images";

			case (static_cast<int>(Columns::Size)):
				return "Size";

			case (static_cast<int>(Columns::NoPoints)):
				return "No. Points";

			case (static_cast<int>(Columns::NoDimensions)):
				return "No. Dimensions";

			case (static_cast<int>(Columns::ImageID)):
				return "Image ID";

			case (static_cast<int>(Columns::DimensionID)):
				return "Dimension ID";

			case (static_cast<int>(Columns::ImageNames)):
				return "Image Names";

			case (static_cast<int>(Columns::DimensionNames)):
				return "Dimension Names";

			case (static_cast<int>(Columns::AverageImages)):
				return "Average Images";

			default:
				return QVariant();
		}
	}

	return QVariant();
}

QModelIndex DatasetsModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	/*
	TreeItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	*/

	return QModelIndex();
}

QModelIndex DatasetsModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

Qt::ItemFlags DatasetsModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool DatasetsModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto imageDataset = _mainModel->imageDatasets().value(row);

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				imageDataset._name = value.toString();
				break;

			case (static_cast<int>(Columns::Type)):
				imageDataset._type = value.toString();
				break;

			case (static_cast<int>(Columns::NoImages)):
				imageDataset._noImages = value.toInt();
				break;

			case (static_cast<int>(Columns::Size)):
				imageDataset._size = value.toSize();
				break;

			case (static_cast<int>(Columns::NoPoints)):
				imageDataset._noPoints = value.toInt();
				break;

			case (static_cast<int>(Columns::NoDimensions)):
				imageDataset._noDimensions = value.toInt();
				break;

			case (static_cast<int>(Columns::ImageID)):
				imageDataset._imageID = value.toInt();
				break;

			case (static_cast<int>(Columns::DimensionID)):
				imageDataset._dimensionID = value.toInt();
				break;

			case (static_cast<int>(Columns::ImageNames)):
				imageDataset._imageNames = value.toStringList();
				break;

			case (static_cast<int>(Columns::DimensionNames)):
				imageDataset._dimensionNames = value.toStringList();
				break;

			case (static_cast<int>(Columns::AverageImages)):
				imageDataset._averageImages = value.toBool();
				break;

			default:
				return false;
		}

		_mainModel->imageDatasets().replace(row, imageDataset);

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

bool DatasetsModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_mainModel->imageDatasets().insert(position, ImageDataset());
	}

	endInsertRows();

	return true;
}

bool DatasetsModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_mainModel->imageDatasets().removeAt(position);
	}

	endRemoveRows();

	return true;
}

QList<DatasetsModel::ImageDataset> DatasetsModel::datasets()
{
	return _mainModel->imageDatasets();
}