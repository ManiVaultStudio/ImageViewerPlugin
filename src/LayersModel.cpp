#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "Dataset.h"
#include "Layer.h"
#include "GroupLayer.h"

#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractItemModel(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_layers(),
	_selectionModel(this),
	_rootItem(new GroupLayer("root", "Root", static_cast<int>(Layer::Flag::Enabled)))
{
}

LayersModel::~LayersModel()
{
	delete _rootItem;
}

int LayersModel::rowCount(const QModelIndex& parentIndex /*= QModelIndex()*/) const
{
	Layer* parentItem = nullptr;

	if (parentIndex.column() > 0)
		return 0;

	if (!parentIndex.isValid())
		parentItem = _rootItem;
	else
		parentItem = static_cast<Layer*>(parentIndex.internalPointer());

	return parentItem->childCount();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return static_cast<int>(Layer::Column::End);
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Layer* parentItem = nullptr;

	if (!parent.isValid())
		parentItem = _rootItem;
	else
		parentItem = static_cast<Layer*>(parent.internalPointer());

	Layer* childItem = parentItem->child(row);

	if (childItem)
		return createIndex(row, column, childItem);

	return QModelIndex();
}

QModelIndex LayersModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto childItem	= static_cast<Layer*>(index.internalPointer());
	auto parentItem	= childItem->parent();

	if (parentItem == _rootItem)
		return QModelIndex();

	return createIndex(childItem->row(), 0, const_cast<Layer*>(parentItem));
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto item = reinterpret_cast<Layer*>(index.internalPointer());

	return item->data(index, role);
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return Layer::columnName(static_cast<Layer::Column>(section));

	return QVariant();
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
	return Qt::NoItemFlags;

	if (!index.isValid())
		return Qt::NoItemFlags;

	return index.flags();
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (!index.isValid())
		return false;

	_layers.value(index.row())->setData(index, value, role);

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
	if (!beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild))
		return false;

	for (int i = 0; i < count; ++i) {
		_layers.insert(destinationChild + i, _layers.at(sourceRow));
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		_layers.removeAt(removeIndex);
	}

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

	auto itemA = static_cast<Layer*>(index(row, 0).internalPointer());
	auto itemB = static_cast<Layer*>(index(row - 1, 0).internalPointer());
	
	if (itemA->flag(Layer::Flag::Frozen, Qt::EditRole).toBool() || itemB->flag(Layer::Flag::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	auto itemA = static_cast<Layer*>(index(row, 0).internalPointer());
	auto itemB = static_cast<Layer*>(index(row + 1, 0).internalPointer());

	if (itemA->flag(Layer::Flag::Frozen, Qt::EditRole).toBool() || itemB->flag(Layer::Flag::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

void LayersModel::moveUp(const int& row)
{
	if (!mayMoveUp(row))
		return;

	moveRows(QModelIndex(), row, 1, QModelIndex(), row - 1);
	//sortOrder();
}

void LayersModel::moveDown(const int& row)
{
	if (!mayMoveDown(row))
		return;

	moveRows(QModelIndex(), row + 1, 1, QModelIndex(), row);
	//sortOrder();
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
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (_layers.at(row)->flag(Layer::Flag::Removable, Qt::EditRole).toBool()) {
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
	/*
	const auto hits = match(index(0, static_cast<int>(LayerItem::Column::ID)), Qt::EditRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	const auto firstHit = hits.first();

	setData(index(firstHit.row(), to_underlying(LayerItem::Column::Name)), name);
	*/
}

Layer* LayersModel::findLayerById(const QString& id)
{
	const auto hits = match(index(0, static_cast<int>(Layer::Column::ID)), Qt::DisplayRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return nullptr;

	return reinterpret_cast<Layer*>(hits.first().internalPointer());
}

void LayersModel::addLayer(Layer* layer, const QModelIndex& parentIndex /*= QModelIndex()*/)
{
	Layer* parentItem = nullptr;
	
	if (!parentIndex.isValid())
		parentItem = _rootItem;
	else
		parentItem = static_cast<Layer*>(parentIndex.internalPointer());

	beginInsertRows(parentIndex, 0, 0);

	parentItem->appendChild(layer);

	endInsertRows();
}