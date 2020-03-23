#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "GeneralSettings.h"

#include <QItemSelectionModel>
#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractListModel(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_layers(),
	_selectionModel()
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return _layers.size();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 20;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= rowCount() || index.row() < 0)
		return QVariant();

	return _layers.at(index.row())->data(static_cast<Layer::Column>(index.column()), role);
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	return Layer::headerData(section, Qt::Horizontal, Qt::DisplayRole);
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return _layers.at(index.row())->itemFlags(static_cast<Layer::Column>(index.column()));
}

Qt::ItemFlags LayersModel::flags(const int& row, const Layer::Column& column) const
{
	return flags(index(row, static_cast<int>(column)));
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (!index.isValid())
		return false;

	_layers.value(index.row())->setData(static_cast<Layer::Column>(index.column()), value, role);

	emit dataChanged(index, index);

	return true;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	/*
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_layers.insert(position, new Layer(_imageViewerPlugin));
	}

	endInsertRows();
	*/

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_layers.removeAt(position);
	}

	endRemoveRows();

	return true;
}

bool LayersModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
	beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

	for (int i = 0; i < count; ++i) {
		_layers.insert(destinationChild + i, _layers.at(sourceRow));
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		_layers.removeAt(removeIndex);
	}
	
	endMoveRows();

	return true;
}

QVariant LayersModel::data(const int& row, const Layer::Column& column, int role) const
{
	const auto modelIndex = index(row, static_cast<int>(column));

	if (!modelIndex.isValid())
		return QVariant();

	return data(index(row, static_cast<int>(column)), role);
}

void LayersModel::setData(const int& row, const Layer::Column& column, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	const auto modelIndex = index(row, static_cast<int>(column));

	if (!modelIndex.isValid())
		return;

	setData(modelIndex, value, role);
}

bool LayersModel::mayMoveUp(const int& row)
{
	if (row <= 0)
		return false;

	if (_layers.at(row)->general()->flag(Layer::Flag::Frozen, Qt::EditRole).toBool() || _layers.at(row - 1)->general()->flag(Layer::Flag::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	if (_layers.at(row)->general()->flag(Layer::Flag::Frozen, Qt::EditRole).toBool() || _layers.at(row + 1)->general()->flag(Layer::Flag::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

void LayersModel::moveUp(const int& row)
{
	if (!mayMoveUp(row))
		return;

	moveRows(QModelIndex(), row, 1, QModelIndex(), row - 1);
	sortOrder();
}

void LayersModel::moveDown(const int& row)
{
	if (!mayMoveDown(row))
		return;

	moveRows(QModelIndex(), row + 1, 1, QModelIndex(), row);
	sortOrder();
}

void LayersModel::sortOrder()
{
	for (int row = 0; row < rowCount(); row++)
		setData(row, Layer::Column::Order, rowCount() - row);
}

void LayersModel::removeRows(const QModelIndexList& rows)
{
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (_layers.at(row)->general()->flag(Layer::Flag::Removable, Qt::EditRole).toBool()) {
			rowsToRemove.append(row);
		}
	}

	if (rowsToRemove.isEmpty())
		return;

	std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());

	beginRemoveRows(QModelIndex(), rowsToRemove.last(), rowsToRemove.first());

	for (auto rowToRemove : rowsToRemove) {
		_layers.removeAt(rowToRemove);
	}

	endRemoveRows();

	sortOrder();
}

void LayersModel::renameLayer(const QString& id, const QString& name)
{
	const auto hits = match(index(0, static_cast<int>(Layer::Column::ID)), Qt::EditRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	const auto firstHit = hits.first();

	setData(firstHit.row(), Layer::Column::Name, name);
}

Layer* LayersModel::findLayerById(const QString& id)
{
	const auto hits = match(index(0, static_cast<int>(Layer::Column::ID)), Qt::DisplayRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return nullptr;

	return _layers[hits.first().row()];
}

void LayersModel::addLayer(Layer* layer)
{
	beginInsertRows(QModelIndex(), 0, 0);

	_layers.insert(0, layer);

	endInsertRows();

	sortOrder();
}