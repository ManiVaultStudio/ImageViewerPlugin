#include "GroupLayer.h"

#include <QDebug>

GroupLayer::GroupLayer(const QString& id, const QString& name, const int& flags) :
	LayerNode(nullptr, LayerNode::Type::Group, id, name, flags)
{
}

int GroupLayer::noColumns() const
{
	return ult(Column::End);
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

QModelIndexList GroupLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (isBaseLayerIndex(index))
		return LayerNode::setData(index, value, role);
}