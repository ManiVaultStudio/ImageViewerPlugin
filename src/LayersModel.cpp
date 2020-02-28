#include "LayersModel.h"

#include <QDebug>

LayersModel::LayersModel(MainModel* mainModel) :
	QAbstractListModel(mainModel),
	_mainModel(mainModel),
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

	if (index.row() >= layers().size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole) {
		auto layer = layers().at(index.row());

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)) :
				return layer._name;

			case (static_cast<int>(Columns::Order)):
				return layer._order;

			case (static_cast<int>(Columns::Opacity)):
				return QString::number(layer._opacity, 'f', 1);

			case (static_cast<int>(Columns::Window)):
				return QString::number(layer._window, 'f', 1);

			case (static_cast<int>(Columns::Level)):
				return QString::number(layer._level, 'f', 1);

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

Qt::ItemFlags LayersModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto layer = layers().value(row);

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)):
				layer._name = value.toString();
				break;

			case (static_cast<int>(Columns::Order)):
				layer._order = value.toInt();
				break;

			case (static_cast<int>(Columns::Opacity)):
				layer._opacity = value.toFloat();
				break;

			case (static_cast<int>(Columns::Window)):
				layer._window = value.toFloat();
				break;

			case (static_cast<int>(Columns::Level)):
				layer._level = value.toFloat();
				break;

			default:
				return false;
		}

		layers().replace(row, layer);

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
		layers().insert(position, MainModel::Dataset::Layer());
	}

	endInsertRows();

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		layers().removeAt(position);
	}

	endRemoveRows();

	return true;
}

const Layers LayersModel::layers() const
{
	return _mainModel->datasets()->at(0)._layers;
}

Layers LayersModel::layers()
{
	const auto constThis = const_cast<const LayersModel*>(this);
	return const_cast<QList<MainModel::Dataset::Layer>&>(constThis->layers());
}