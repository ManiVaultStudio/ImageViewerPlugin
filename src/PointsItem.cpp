#include "PointsItem.h"
#include "LayerItem.h"

#include <QDebug>

PointsItem::PointsItem(LayerItem* layeritem) :
	TreeItem(layeritem)
{
}

int PointsItem::columnCount() const
{
	return 0;
}

QVariant PointsItem::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	return QVariant();
}

Qt::ItemFlags PointsItem::flags(const int& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return flags;
}

QVariant PointsItem::data(const int& column, const int& role) const
{
	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return QVariant();
}

void PointsItem::setData(const int& column, const QVariant& value, const int& role)
{
}