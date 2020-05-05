#include "ClustersLayer.h"

#include <QDebug>

ClustersLayer::ClustersLayer(const QString& clusterDatasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(clusterDatasetName, LayerNode::Type::Clusters, id, name, flags),
	_clustersDatasetName(clusterDatasetName),
	_clustersDataset(nullptr)
{
}

Qt::ItemFlags ClustersLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

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
		return LayerNode::data(index, role);

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
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

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

void ClustersLayer::mousePressEvent(QMouseEvent* mouseEvent)
{
}

void ClustersLayer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
}

void ClustersLayer::mouseMoveEvent(QMouseEvent* mouseEvent)
{
}

void ClustersLayer::mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index)
{
}

void ClustersLayer::keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}

void ClustersLayer::keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}