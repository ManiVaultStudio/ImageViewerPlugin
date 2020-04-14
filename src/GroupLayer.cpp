#include "GroupLayer.h"

#include <QDebug>

GroupLayer::GroupLayer(const QString& id, const QString& name, const int& flags) :
	LayerNode("", LayerNode::Type::Group, id, name, flags)
{
}

int GroupLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags GroupLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant GroupLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

QModelIndexList GroupLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (index.column() < ult(Column::Start))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIndices{ index };

	return affectedIndices;
}