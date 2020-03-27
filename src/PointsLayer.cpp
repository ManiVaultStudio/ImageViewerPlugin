#include "PointsLayer.h"
#include "PointsDataset.h"

#include <QDebug>

PointsLayer::PointsLayer(Layer* parent, PointsDataset* pointsDataset, const QString& id, const QString& name, const int& flags) :
	Layer(parent, pointsDataset, Layer::Type::Points, id, name, flags),
	_points(pointsDataset)
{
}

Qt::ItemFlags PointsLayer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant PointsLayer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
}