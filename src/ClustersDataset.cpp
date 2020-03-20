#include "ClustersDataset.h"

#include <QDebug>

ClustersDataset::ClustersDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name) :
	Dataset(imageViewerPlugin, name, Type::Clusters)
{
}

void ClustersDataset::init()
{
}

int ClustersDataset::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent)

	return 0;
}

QVariant ClustersDataset::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

Qt::ItemFlags ClustersDataset::flags(const QModelIndex &index) const
{
	return 0;
}

QVariant ClustersDataset::data(const int& row, const int& column, int role) const
{
	return QVariant();
}

void ClustersDataset::setData(const int& row, const int& column, const QVariant& value)
{

}