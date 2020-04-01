#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "Dataset.h"
#include "Layer.h"
#include "GroupLayer.h"
#include "PointsLayer.h"
#include "ImagesLayer.h"
#include "ClustersLayer.h"

#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>

LayersModel::LayersModel(QObject *parent) :
	QAbstractItemModel(parent),
	_selectionModel(this),
	_root(new GroupLayer("root", "Root", ult(Layer::Flag::Enabled)))
{
	auto pointsLayer	= new PointsLayer(nullptr, "points", "Points", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable));
	auto imagesLayer	= new ImagesLayer(nullptr, "images", "Images", ult(Layer::Flag::Enabled));
	auto clusterLayer	= new ClustersLayer(nullptr, "clusters", "Clusters", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable));

	pointsLayer->insertChild(0, imagesLayer);
	pointsLayer->insertChild(0, clusterLayer);

	_root->insertChild(0, pointsLayer);
	_root->insertChild(0, new ImagesLayer(nullptr, "images", "Images", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable)));
	_root->insertChild(0, new ClustersLayer(nullptr, "clusters", "Clusters", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable)));
	_root->insertChild(0, new ImagesLayer(nullptr, "images", "Images", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable)));
	_root->insertChild(0, new ClustersLayer(nullptr, "clusters", "Clusters", ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable)));
}

LayersModel::~LayersModel()
{
	delete _root;
}

int LayersModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return ult(Layer::Column::End);
}

QVariant LayersModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto layer = getLayer(index);

	return layer->data(index, role);
}

QVariant LayersModel::data(const int& row, const int& column, const int& role) const
{
	return data(index(row, column), role);
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
	auto layer = getLayer(index);

	layer->setData(index, value, role);

	if (index.column() == ult(Layer::Column::Enabled))
		emit dataChanged(this->index(index.row(), 0), this->index(index.row(), layer->noColumns() - 1));
	else 
		emit dataChanged(index, index);

	//emit dataChanged(index, index);

	return true;
}

bool LayersModel::setData(const int& row, const int& column, const QVariant& value, int role /*= Qt::EditRole*/)
{
	return setData(index(row, column), value, role);
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
	int itemFlags = QAbstractItemModel::flags(index);

	if (index == QModelIndex())
		return itemFlags | Qt::ItemIsDropEnabled;

	if (!index.isValid())
		return itemFlags | Qt::ItemIsDropEnabled;

	if (getLayer(index)->childCount() > 0)
		itemFlags |= Qt::ItemIsDropEnabled;

	//itemFlags |= Qt::ItemIsDropEnabled;

	return itemFlags | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
}

Qt::ItemFlags LayersModel::flags(const int& row, const int& column) const
{
	return flags(index(row, column));
}

Layer* LayersModel::getLayer(const QModelIndex& index) const
{
	if (index.isValid()) {
		auto layer = static_cast<Layer*>(index.internalPointer());
		
		if (layer)
			return layer;
	}

	return _root;
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		return Layer::columnName(static_cast<Layer::Column>(section));
	}

	return QVariant();
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	auto parentLayer = getLayer(parent);

	if (!parentLayer)
		return QModelIndex();

	auto childLayer = parentLayer->child(row);

	if (childLayer)
		return createIndex(row, column, childLayer);

	return QModelIndex();
}

bool LayersModel::insertLayer(int row, Layer* layer, const QModelIndex& parent /*= QModelIndex()*/)
{
	auto parentLayer = getLayer(parent);

	if (!parentLayer)
		return false;

	beginInsertRows(parent, row, row);
	{
		parentLayer->insertChild(row, layer);
	}
	endInsertRows();

	return true;
}

QModelIndex LayersModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto childLayer		= getLayer(index);
	auto parentLayer	= childLayer ? childLayer->parent() : nullptr;

	if (parentLayer == _root || !parentLayer)
		return QModelIndex();

	return createIndex(parentLayer->childIndex(), 0, parentLayer);
}

bool LayersModel::removeLayer(const QModelIndex& index)
{
	const auto row = index.row();

	beginRemoveRows(index.parent(), row, row);
	{
		getLayer(index.parent())->removeChild(row);
	}
	endRemoveRows();
	
	return true;
}

bool LayersModel::mayMoveLayer(const QModelIndex& index, const int& delta) const
{
	const auto sourceIndex = index;
	const auto targetIndex = index.siblingAtRow(index.row() + delta);

	if (!sourceIndex.isValid() || !targetIndex.isValid())
		return false;

	return true;
}

bool LayersModel::moveLayer(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow)
{
	if (targetRow < 0)
		targetRow = 0;

	if (sourceParent == targetParent) {
		if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
			auto sourceParentLayer	= getLayer(sourceParent);
			auto targetParentLayer	= getLayer(targetParent);
			auto sourceLayer		= sourceParentLayer->child(sourceRow);

			sourceParentLayer->removeChild(sourceRow, false);
			targetParentLayer->insertChild(targetRow > sourceRow ? targetRow - 1 : targetRow, sourceLayer);

			endMoveRows();
		}
	}
	else {
		if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
			auto sourceParentLayer	= getLayer(sourceParent);
			auto targetParentLayer	= getLayer(targetParent);
			auto sourceLayer		= sourceParentLayer->child(sourceRow);

			sourceParentLayer->removeChild(sourceRow, false);
			targetParentLayer->insertChild(targetRow, sourceLayer);

			endMoveRows();
		}
	}

	return true;
}

int LayersModel::rowCount(const QModelIndex &parent) const
{
	const auto parentLayer = getLayer(parent);

	return parentLayer ? parentLayer->childCount() : 0;
}

QStringList LayersModel::mimeTypes() const
{
	QStringList types;

	types << "layer";

	return types;
}

QMimeData* LayersModel::mimeData(const QModelIndexList& indexes) const
{
	if (!indexes[0].isValid())
		return nullptr;

	const auto index = indexes[0];

	const auto layerAddress = (quintptr)indexes[0].internalPointer();

	auto mimeData = new QMimeData();

	QByteArray encodedData;

	QDataStream dataStream(&encodedData, QIODevice::WriteOnly);

	dataStream << index.row() << index.column() << layerAddress;

	mimeData->setData("layer", encodedData);

	return mimeData;
}

bool LayersModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (!canDropMimeData(data, action, row, column, parent))
		return false;

	switch (action) {
		case Qt::IgnoreAction:
			break;

		case Qt::MoveAction:
		{
			QByteArray bytes = data->data("layer");
			QDataStream stream(&bytes, QIODevice::QIODevice::ReadOnly);
			
			qintptr sourceInternalPointer;
			int sourceRow;
			int sourceColumn;

			stream >> sourceRow >> sourceColumn >> sourceInternalPointer;

			QModelIndex index = createIndex(sourceRow, sourceColumn, sourceInternalPointer);

			moveLayer(index.parent(), index.row(), parent, row);
			
			break;
		}

		default:
			break;
	}

	return true;
}