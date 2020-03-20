#include "PointsDataset.h"
#include "ImageViewerPlugin.h"

#include <QDebug>

PointsDataset::PointsDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name) :
	Dataset(imageViewerPlugin, name, Type::Points)
{
}

void PointsDataset::init()
{
}

int PointsDataset::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent)

	return 0;
}

QVariant PointsDataset::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

Qt::ItemFlags PointsDataset::flags(const QModelIndex &index) const
{
	return 0;
}

QVariant PointsDataset::data(const int& row, const int& column, int role) const
{
	return QVariant();
}

void PointsDataset::setData(const int& row, const int& column, const QVariant& value)
{
}