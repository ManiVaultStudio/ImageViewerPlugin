#include "RootItem.h"

#include <QDebug>

RootItem::RootItem() :
	Item()
{
}

int RootItem::columnCount() const
{
	return 0;
}

QVariant RootItem::headerData(const int& section, const Qt::Orientation& orientation, const int& role) const
{
	return QVariant();
}

Qt::ItemFlags RootItem::flags(const int& column) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return flags;
}

QVariant RootItem::data(const int& column, const int& role) const
{
	switch (static_cast<Column>(column)) {
		default:
			break;
	}

	return QVariant();
}

void RootItem::setData(const int& column, const QVariant& value, const int& role)
{
}