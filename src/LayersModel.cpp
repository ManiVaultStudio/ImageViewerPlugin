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
	_selectionModel(this),
	_rootItem(new GroupLayer("root", "Root", static_cast<int>(Layer::Flag::Enabled)))
{
	_selectionModel.setModel(&_proxyModel);
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
	if (!index.isValid())
		return Qt::NoItemFlags;

	auto item = reinterpret_cast<Layer*>(index.internalPointer());

	return item->flags(index);
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (!index.isValid())
		return false;

	auto item = reinterpret_cast<Layer*>(index.internalPointer());

	item->setData(index, value, Qt::DisplayRole);

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

int LayersModel::order(const QModelIndex& layerIndex) const
{
	if (!layerIndex.isValid())
		return -1;

	return layerIndex.siblingAtColumn(to_underlying(Layer::Column::Order)).data(Qt::EditRole).toInt();
}

int LayersModel::noSiblings(const QModelIndex& layerIndex) const
{
	return rowCount(layerIndex.parent());
}

bool LayersModel::mayMoveUp(const QModelIndex& layerIndex) const
{
	if (!layerIndex.isValid())
		return false;

	if (order(layerIndex) >= noSiblings(layerIndex) - 1)
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const QModelIndex& layerIndex) const
{
	if (!layerIndex.isValid())
		return false;

	if (order(layerIndex) <= 0)
		return false;
	
	return true;
}

void LayersModel::moveUp(const QModelIndex& layerIndex)
{
	if (!mayMoveUp(layerIndex))
		return;

	const auto indexA = layerIndex;
	const auto indexB = layerIndex.siblingAtRow(layerIndex.row() - 1);

	const auto orderA = order(indexA);
	const auto orderB = order(indexB);

	const auto parent = layerIndex.parent();

	beginMoveRows(parent, indexA.row(), indexA.row(), parent, indexB.row());

	setData(indexA, orderB, Qt::EditRole);
	setData(indexB, orderA, Qt::EditRole);

	endMoveRows();
}

void LayersModel::moveDown(const QModelIndex& layerIndex)
{
	if (!mayMoveUp(layerIndex))
		return;

	const auto indexA = layerIndex;
	const auto indexB = layerIndex.siblingAtRow(layerIndex.row() + 1);

	const auto orderA = order(indexA);
	const auto orderB = order(indexB);

	const auto parent = layerIndex.parent();

	beginMoveRows(parent, indexA.row(), indexA.row(), parent, indexB.row());

	setData(indexA, orderB, Qt::EditRole);
	setData(indexB, orderA, Qt::EditRole);

	endMoveRows();
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