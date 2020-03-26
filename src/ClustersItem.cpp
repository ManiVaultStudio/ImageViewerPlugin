#include "ClustersItem.h"
#include "LayerItem.h"

#include <QDebug>

ClustersItem::ClustersItem(LayerItem* layeritem) :
	TreeItem(layeritem)
{
}

int ClustersItem::columnCount() const
{
	return 0;
}

QVariant ClustersItem::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	return QVariant();
}

Qt::ItemFlags ClustersItem::flags(const int& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return flags;
}

QVariant ClustersItem::data(const int& column, const int& role) const
{
	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return QVariant();
}

void ClustersItem::setData(const int& column, const QVariant& value, const int& role)
{
}