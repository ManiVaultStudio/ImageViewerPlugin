#include "ClustersLayer.h"

#include <QDebug>

ClustersLayer::ClustersLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags) :
	Layer(dataset, Type::Clusters, id, name, flags)
{
}

Qt::ItemFlags ClustersLayer::itemFlags(const int& column) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::itemFlags(column);

	return 0;
}

QVariant ClustersLayer::data(const int& column, int role) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::data(column, role);

	return QVariant();
}

void ClustersLayer::setData(const int& column, const QVariant& value, const int& role)
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::setData(column, value, role);
}