#include "SelectionLayer.h"

#include <QDebug>

SelectionLayer::SelectionLayer(const QString& dataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(dataset, LayerNode::Type::Selection, id, name, flags)
{
}

int SelectionLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

QModelIndexList SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (index.column() < ult(Column::Start))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIndices{ index };

	return affectedIndices;
}