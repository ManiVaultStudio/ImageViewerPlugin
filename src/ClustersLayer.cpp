#include "ClustersLayer.h"

#include <QDebug>

ClustersLayer::ClustersLayer(const QString& dataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(dataset, LayerNode::Type::Clusters, id, name, flags)
{
}

int ClustersLayer::noColumns() const
{
	return ult(Column::End);
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
	if (index.column() < ult(Column::Start))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIndices{ index };

	return affectedIndices;
}