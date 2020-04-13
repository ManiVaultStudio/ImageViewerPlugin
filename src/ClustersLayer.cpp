#include "ClustersLayer.h"
#include "ClustersDataset.h"

#include <QDebug>

ClustersLayer::ClustersLayer(ClustersDataset* clustersDataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(clustersDataset, LayerNode::Type::Clusters, id, name, flags),
	_clusters(clustersDataset)
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
	if (isBaseLayerIndex(index))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

QModelIndexList ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (isBaseLayerIndex(index))
		return LayerNode::setData(index, value, role);
}