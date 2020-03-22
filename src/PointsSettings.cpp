#include "PointsSettings.h"

#include <QDebug>

PointsSettings::PointsSettings(Dataset* dataset) :
	Settings(dataset)
{
}

Qt::ItemFlags PointsSettings::itemFlags(const LayerColumn& column) const
{
	return 0;
}

QVariant PointsSettings::data(const LayerColumn& column, int role) const
{
	return QVariant();
}

void PointsSettings::setData(const LayerColumn& column, const QVariant& value, const int& role)
{
}