#include "ClustersLayer.h"
#include "ClustersDataset.h"

#include <QDebug>

ClustersLayer::ClustersLayer(ClustersDataset* clustersDataset, const QString& id, const QString& name, const int& flags) :
	Layer(clustersDataset, Layer::Type::Clusters, id, name, flags),
	_clusters(clustersDataset)
{
}

int ClustersLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

	if (index.column() == ult(Layer::Column::WindowNormalized) || index.column() == ult(Layer::Column::LevelNormalized))
		flags &= ~Qt::ItemIsEditable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant ClustersLayer::data(const QModelIndex& index, const int& role) const
{
	if (isBaseLayerIndex(index))
		return Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (isBaseLayerIndex(index))
		return Layer::setData(index, value, role);
}