#include "PointsSettings.h"

#include <QDebug>

PointsSettings::PointsSettings(QObject* parent, Dataset* dataset) :
	Settings(parent, dataset)
{
}

Qt::ItemFlags PointsSettings::itemFlags(const Layer::Column& column) const
{
	return 0;
}

QVariant PointsSettings::data(const Layer::Column& column, int role) const
{
	return QVariant();
}

void PointsSettings::setData(const Layer::Column& column, const QVariant& value, const int& role)
{
}