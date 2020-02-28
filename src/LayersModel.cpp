#include "LayersModel.h"
#include "MainModel.h"

#include <QDebug>

LayersModel::LayersModel(MainModel* mainModel) :
	QAbstractListModel(mainModel),
	_currentDatasetName()
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return 0;
}

int LayersModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 11;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _imageLayers.size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole) {
		auto imageLayer = _imageLayers.at(index.row());

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)) :
				return imageLayer._name;

			case (static_cast<int>(Columns::Order)):
				return imageLayer._order;

			case (static_cast<int>(Columns::Opacity)):
				return QString::number(imageLayer._opacity, 'f', 1);

			case (static_cast<int>(Columns::Window)):
				return QString::number(imageLayer._window, 'f', 1);

			case (static_cast<int>(Columns::Level)):
				return QString::number(imageLayer._level, 'f', 1);

			default:
				break;
		}
	}

	return QVariant();
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case (static_cast<int>(Columns::Name)):
				return "Name";

			case (static_cast<int>(Columns::Order)):
				return "Type";

			case (static_cast<int>(Columns::Opacity)):
				return "Opacity";

			case (static_cast<int>(Columns::Window)):
				return "Window";

			case (static_cast<int>(Columns::Level)):
				return "Level";

			default:
				return QVariant();
		}
	}

	return QVariant();
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
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

QModelIndex LayersModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

Qt::ItemFlags LayersModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto imageLayer = _imageLayers.value(row);

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				imageLayer._name = value.toString();
				break;

			case (static_cast<int>(Columns::Order)):
				imageLayer._order = value.toInt();
				break;

			case (static_cast<int>(Columns::Opacity)):
				imageLayer._opacity = value.toFloat();
				break;

			case (static_cast<int>(Columns::Window)):
				imageLayer._window = value.toFloat();
				break;

			case (static_cast<int>(Columns::Level)):
				imageLayer._level = value.toFloat();
				break;

			default:
				return false;
		}

		_imageLayers.replace(row, imageLayer);

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_imageLayers.insert(position, ImageLayer());
	}

	endInsertRows();

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_imageLayers.removeAt(position);
	}

	endRemoveRows();

	return true;
}

QList<LayersModel::ImageLayer> LayersModel::layers()
{
	return _imageLayers;
}