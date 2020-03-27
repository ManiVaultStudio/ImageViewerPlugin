#include "ClustersLayer.h"
#include "ClustersDataset.h"

#include <QDebug>

ClustersLayer::ClustersLayer(Layer* parent, ClustersDataset* clustersDataset, const QString& id, const QString& name, const int& flags) :
	Layer(parent, clustersDataset, Layer::Type::Clusters, id, name, flags),
	_clusters(clustersDataset)
{
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant ClustersLayer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
}