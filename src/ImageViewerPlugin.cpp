#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <vector>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::~ImageViewerPlugin(void)
{
}

void ImageViewerPlugin::init()
{
}

void ImageViewerPlugin::dataAdded(const QString name)
{
}

void ImageViewerPlugin::dataChanged(const QString name)
{
}

void ImageViewerPlugin::dataRemoved(const QString name)
{
}

void ImageViewerPlugin::selectionChanged(const QString dataName)
{
}

QStringList ImageViewerPlugin::supportedDataKinds()
{
	QStringList supportedKinds;

	supportedKinds << "Points";
	
	return supportedKinds;
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}