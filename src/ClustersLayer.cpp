#include "ClustersLayer.h"

#include <QDebug>

ClustersLayer::ClustersLayer(const QString& clusterDatasetName, const QString& id, const QString& name, const int& flags) :
	Layer(clusterDatasetName, Layer::Type::Clusters, id, name, flags),
	Channels<std::uint16_t>(1),
	_clustersDatasetName(clusterDatasetName),
	_clustersDataset(nullptr)
{
}

void ClustersLayer::paint(QPainter* painter)
{
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return flags;
}

QVariant ClustersLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return Layer::data(index, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return QVariant();
}

QModelIndexList ClustersLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = Layer::setData(index, value, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return affectedIds;
}

QVariant ClustersLayer::clustersDatasetName(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return _clustersDatasetName;

		case Qt::EditRole:
			return _clustersDatasetName;

		case Qt::ToolTipRole:
			return QString("Clusters dataset name: %1").arg(_clustersDatasetName);

		default:
			break;
	}

	return QVariant();
}

Layer::Hints ClustersLayer::hints() const
{
	return Layer::hints();
}