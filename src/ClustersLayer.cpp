#include "ClustersLayer.h"

#include <QDebug>

ClustersLayer::ClustersLayer(const QString& dataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(dataset, LayerNode::Type::Clusters, id, name, flags)
{
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant ClustersLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

QModelIndexList ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return affectedIds;
}