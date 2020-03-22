#include "ClustersSettings.h"

#include <QDebug>

ClustersSettings::ClustersSettings(Dataset* dataset) :
	Settings(dataset)
{
}

Qt::ItemFlags ClustersSettings::itemFlags(const LayerColumn& column) const
{
	return 0;
}

QVariant ClustersSettings::data(const LayerColumn& column, int role) const
{
	return QVariant();
}

void ClustersSettings::setData(const LayerColumn& column, const QVariant& value, const int& role)
{
}