#include "GroupLayer.h"

#include <QDebug>

GroupLayer::GroupLayer(const QString& id, const QString& name, const int& flags) :
	Layer(nullptr, Layer::Type::Group, id, name, flags)
{
}

Qt::ItemFlags GroupLayer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant GroupLayer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void GroupLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
}