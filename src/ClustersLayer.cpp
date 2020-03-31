#include "ClustersLayer.h"
#include "ClustersDataset.h"

#include <QDebug>

ClustersLayer::ClustersLayer(ClustersDataset* clustersDataset, const QString& id, const QString& name, const int& flags) :
	_Layer(clustersDataset, _Layer::Type::Clusters, id, name, flags),
	_clusters(clustersDataset)
{
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	if (!isSettingsIndex(index))
		return _Layer::flags(index);

	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant ClustersLayer::data(const QModelIndex& index, const int& role) const
{
	if (!isSettingsIndex(index))
		return _Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (!isSettingsIndex(index))
		return _Layer::setData(index, value, role);
}