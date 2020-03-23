#include "Pointslayer.h"

#include <QDebug>

PointsLayer::PointsLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags) :
	Layer(dataset, Type::Points, id, name, flags)
{
}

Qt::ItemFlags PointsLayer::itemFlags(const int& column) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::itemFlags(column);

	return 0;
}

QVariant PointsLayer::data(const int& column, int role) const
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::data(column, role);

	return QVariant();
}

void PointsLayer::setData(const int& column, const QVariant& value, const int& role)
{
	if (column < static_cast<int>(Layer::Column::Count))
		return Layer::setData(column, value, role);
}