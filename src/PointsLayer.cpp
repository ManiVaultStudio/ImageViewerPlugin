#include "PointsLayer.h"
#include "PointsDataset.h"

#include <QDebug>

PointsLayer::PointsLayer(PointsDataset* pointsDataset, const QString& id, const QString& name, const int& flags) :
	_Layer(pointsDataset, _Layer::Type::Points, id, name, flags),
	_points(pointsDataset)
{
}

Qt::ItemFlags PointsLayer::flags(const QModelIndex& index) const
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

QVariant PointsLayer::data(const QModelIndex& index, const int& role) const
{
	if (!isSettingsIndex(index))
		return _Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (!isSettingsIndex(index))
		return _Layer::setData(index, value, role);
}