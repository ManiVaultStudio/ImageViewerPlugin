#include "SelectionItem.h"
#include "LayerItem.h"
#include "ClustersDataset.h"

#include <QDebug>

SelectionItem::SelectionItem(LayerItem* layeritem, Dataset* dataset) :
	Item(layeritem),
	_dataset(dataset)
{
}

int SelectionItem::columnCount() const
{
	return 0;
}

QVariant SelectionItem::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	return QVariant();
}

Qt::ItemFlags SelectionItem::flags(const int& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionItem::data(const int& column, const int& role) const
{
	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return QVariant();
}

void SelectionItem::setData(const int& column, const QVariant& value, const int& role)
{
}