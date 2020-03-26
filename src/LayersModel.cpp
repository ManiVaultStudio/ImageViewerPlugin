#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "RootItem.h"
#include "LayerItem.h"
#include "Dataset.h"

#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractItemModel(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_selectionModel(this),
	_rootItem(new RootItem())
{
}

LayersModel::~LayersModel()
{
	delete _rootItem;
}

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Item* parentItem = nullptr;

	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = _rootItem;
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	return parentItem->childCount();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (parent.isValid())
		return static_cast<Item*>(parent.internalPointer())->columnCount();

	return static_cast<int>(LayerItem::Column::End);
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Item* parentItem = nullptr;

	if (!parent.isValid())
		parentItem = _rootItem;
	else
		parentItem = static_cast<Item*>(parent.internalPointer());

	Item* childItem = parentItem->child(row);

	if (childItem)
		return createIndex(row, column, childItem);

	return QModelIndex();
}

QModelIndex LayersModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto childItem	= static_cast<Item*>(index.internalPointer());
	auto parentItem	= childItem->parentItem();

	if (parentItem == _rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	auto item = static_cast<Item*>(index.internalPointer());

	return item->data(index.column(), role);
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return LayerItem::columnName(static_cast<LayerItem::Column>(section));

	return QVariant();
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	auto item = static_cast<Item*>(index.internalPointer());

	return item->flags(index.column());
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	/*
	if (!index.isValid())
		return false;

	_layers.value(index.row())->setData(index, value, role);

	emit dataChanged(index, index);

	return true;
	*/

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

	/*
	for (int row = 0; row < rows; ++row) {
		_layers.removeAt(position);
	}
	*/

	endRemoveRows();

	return true;
}

bool LayersModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
	beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

	/*
	for (int i = 0; i < count; ++i) {
		_layers.insert(destinationChild + i, _layers.at(sourceRow));
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		_layers.removeAt(removeIndex);
	}
	*/

	endMoveRows();

	return true;
}

QVariant LayersModel::data(const int& row, const int& column, int role, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (parent == QModelIndex())
		return data(index(row, column), role);

	return data(index(row, column, parent), role);
}

void LayersModel::setData(const int& row, const int& column, const QVariant& value, const QModelIndex& parent /*= QModelIndex()*/)
{
	if (parent == QModelIndex())
		setData(index(row, column), value);
	else
		setData(index(row, column, parent), value);
}

bool LayersModel::mayMoveUp(const int& row)
{
	if (row <= 0)
		return false;

	/*
	if (_layers.at(row)->flag(LayerItem::Flag::Frozen, Qt::EditRole).toBool() || _layers.at(row - 1)->flag(LayerItem::Flag::Frozen, Qt::EditRole).toBool())
		return false;
	*/

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	/*
	if (_layers.at(row)->flag(LayerItem::Flag::Frozen, Qt::EditRole).toBool() || _layers.at(row + 1)->flag(LayerItem::Flag::Frozen, Qt::EditRole).toBool())
		return false;
	*/

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
	/*
	for (int row = 0; row < rowCount(); row++)
		setData(index(row, to_underlying(LayerItem::Column::Order)), rowCount() - row);
	*/
}

void LayersModel::removeRows(const QModelIndexList& rows)
{
	/*
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (_layers.at(row)->flag(LayerItem::Flag::Removable, Qt::EditRole).toBool()) {
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
	*/
}

void LayersModel::renameLayer(const QString& id, const QString& name)
{
	/*
	const auto hits = match(index(0, static_cast<int>(LayerItem::Column::ID)), Qt::EditRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	const auto firstHit = hits.first();

	setData(index(firstHit.row(), to_underlying(LayerItem::Column::Name)), name);
	*/
}

LayerItem* LayersModel::findLayerById(const QString& id)
{
	/*
	const auto hits = match(index(0, static_cast<int>(LayerItem::Column::ID)), Qt::DisplayRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return nullptr;

	return _layers[hits.first().row()];
	*/

	return nullptr;
}

void LayersModel::addLayer(Dataset* dataset, const LayerItem::Type& type, const QString& id, const QString& name, const std::uint32_t& flags)
{
	beginInsertRows(QModelIndex(), 0, 0);

	_rootItem->appendChild(new LayerItem(_rootItem, dataset, type, id, name, flags));

	endInsertRows();
}