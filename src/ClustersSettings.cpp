#include "ClustersSettings.h"

#include <QDebug>

ClustersSettings::ClustersSettings(QObject* parent, Dataset* dataset) :
	Settings(parent, dataset)
{
}

Qt::ItemFlags ClustersSettings::itemFlags(const Layer::Column& column) const
{
	return 0;
}

QVariant ClustersSettings::data(const Layer::Column& column, int role) const
{
	return QVariant();
}

void ClustersSettings::setData(const Layer::Column& column, const QVariant& value, const int& role)
{
}