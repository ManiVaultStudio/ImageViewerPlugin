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