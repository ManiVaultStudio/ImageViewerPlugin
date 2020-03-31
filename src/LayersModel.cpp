#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "Dataset.h"
#include "Layer.h"
#include "GroupLayer.h"

#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QAbstractItemView>

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

	rootItem = new TreeItem(rootData);
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

	TreeItem *item = getItem(index);

	return item->data(index.column());
}

Qt::ItemFlags LayersModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeItem *LayersModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
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

	TreeItem *parentItem = getItem(parent);
	if (!parentItem)
		return QModelIndex();

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

bool LayersModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	beginInsertColumns(parent, position, position + columns - 1);
	const bool success = rootItem->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	if (!parentItem)
		return false;

	beginInsertRows(parent, position, position + rows - 1);
	const bool success = parentItem->insertChildren(position,
		rows,
		rootItem->columnCount());
	endInsertRows();

	return success;
}

QModelIndex LayersModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = getItem(index);
	TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

	if (parentItem == rootItem || !parentItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool LayersModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	beginRemoveColumns(parent, position, position + columns - 1);
	const bool success = rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return success;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	if (!parentItem)
		return false;

	beginRemoveRows(parent, position, position + rows - 1);
	const bool success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int LayersModel::rowCount(const QModelIndex &parent) const
{
	const TreeItem *parentItem = getItem(parent);

	return parentItem ? parentItem->childCount() : 0;
}

bool LayersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	TreeItem *item = getItem(index);
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

void LayersModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
	QVector<TreeItem*> parents;
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
			TreeItem *parent = parents.last();
			parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
			for (int column = 0; column < columnData.size(); ++column)
				parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
		}
		++number;
	}
}