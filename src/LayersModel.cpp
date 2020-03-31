#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "Dataset.h"
#include "Layer.h"
#include "GroupLayer.h"

#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>

LayersModel::LayersModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	const QStringList headers({ tr("Title"), tr("Description") });

	QFile file(":/default.txt");
	file.open(QIODevice::ReadOnly);
	auto data = static_cast<QString>(file.readAll());
	file.close();

	QVector<QVariant> rootData;
	for (const QString &header : headers)
		rootData << header;

	rootItem = new Layer(rootData);
	setupModelData(data.split('\n'), rootItem);
}

LayersModel::~LayersModel()
{
	delete rootItem;
}

int LayersModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return rootItem->columnCount();
}

QVariant LayersModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	Layer *item = getItem(index);

	return item->data(index.column());
}

QVariant LayersModel::data(const int& row, const int& column, const int& role) const
{
	return data(index(row, column), role);
}

Qt::ItemFlags LayersModel::flags(const QModelIndex &index) const
{
	int itemFlags = Qt::NoItemFlags;

	if (!index.isValid())
		return itemFlags;

	if (getItem(index)->childCount() > 0)
		itemFlags |= Qt::ItemIsDropEnabled;

	return itemFlags | Qt::ItemIsDragEnabled | Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

Layer *LayersModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		Layer *item = static_cast<Layer*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	Layer *parentItem = getItem(parent);
	if (!parentItem)
		return QModelIndex();

	Layer *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

bool LayersModel::insertLayer(int row, Layer* layer, const QModelIndex& parent /*= QModelIndex()*/)
{
	Layer* parentLayer = getItem(parent);

	if (!parentLayer)
		return false;

	beginInsertRows(parent, row, row);

	const bool success = parentLayer->insertChild(row, layer);

	endInsertRows();

	return success;
}

QModelIndex LayersModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	Layer *childItem = getItem(index);
	Layer *parentItem = childItem ? childItem->parent() : nullptr;

	if (parentItem == rootItem || !parentItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	Layer *parentItem = getItem(parent);
	if (!parentItem)
		return false;

	beginRemoveRows(parent, position, position + rows - 1);
	const bool success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

bool LayersModel::mayMoveRow(const QModelIndex& index, const int& delta) const
{
	const auto sourceIndex = index;
	const auto targetIndex = index.siblingAtRow(index.row() + delta);

	if (!sourceIndex.isValid() || !targetIndex.isValid())
		return false;

	return true;
}

bool LayersModel::moveRow(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow)
{
	if (targetRow < 0)
		targetRow = 0;

	if (sourceParent == targetParent) {
		if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
			Layer* sourceParentLayer = getItem(sourceParent);
			Layer* targetParentLayer = getItem(targetParent);

			auto sourceLayer = sourceParentLayer->child(sourceRow);

			sourceParentLayer->removeChildren(sourceRow, 1, false);
			targetParentLayer->insertChild(targetRow > sourceRow ? targetRow - 1 : targetRow, sourceLayer);

			endMoveRows();
		}
	}
	else {
		if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
			Layer* sourceParentLayer = getItem(sourceParent);
			Layer* targetParentLayer = getItem(targetParent);

			auto sourceLayer = sourceParentLayer->child(sourceRow);

			sourceParentLayer->removeChildren(sourceRow, 1, false);
			targetParentLayer->insertChild(targetRow, sourceLayer);

			endMoveRows();
		}
	}
	

	return true;
}

int LayersModel::rowCount(const QModelIndex &parent) const
{
	const Layer *parentItem = getItem(parent);

	return parentItem ? parentItem->childCount() : 0;
}

bool LayersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	Layer *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

	return result;
}

bool LayersModel::setHeaderData(int section, Qt::Orientation orientation,
	const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	const bool result = rootItem->setData(section, value);

	if (result)
		emit headerDataChanged(orientation, section, section);

	return result;
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

			moveRow(index.parent(), index.row(), parent, row);
			
			break;
		}

		default:
			break;
	}

	return true;
}

void LayersModel::setupModelData(const QStringList &lines, Layer *parent)
{
	QVector<Layer*> parents;
	QVector<int> indentations;
	parents << parent;
	indentations << 0;

	int number = 0;

	while (number < lines.count()) {
		int position = 0;
		while (position < lines[number].length()) {
			if (lines[number].at(position) != ' ')
				break;
			++position;
		}

		const QString lineData = lines[number].mid(position).trimmed();

		if (!lineData.isEmpty()) {
			// Read the column data from the rest of the line.
			const QStringList columnStrings = lineData.split('\t', QString::SkipEmptyParts);
			QVector<QVariant> columnData;
			columnData.reserve(columnStrings.size());
			for (const QString &columnString : columnStrings)
				columnData << columnString;

			if (position > indentations.last()) {
				// The last child of the current parent is now the new parent
				// unless the current parent has no children.

				if (parents.last()->childCount() > 0) {
					parents << parents.last()->child(parents.last()->childCount() - 1);
					indentations << position;
				}
			}
			else {
				while (position < indentations.last() && parents.count() > 0) {
					parents.pop_back();
					indentations.pop_back();
				}
			}

			// Append a new item to the current parent's list of children.
			Layer *parent = parents.last();
			parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
			for (int column = 0; column < columnData.size(); ++column)
				parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
		}
		++number;
	}
}