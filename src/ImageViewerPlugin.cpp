#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "PointsPlugin.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <vector>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

inline ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer")
{
	_imageDataSetsComboBox	= new QComboBox();
	_imagesComboBox			= new QComboBox();
	_imageViewerWidget		= new ImageViewerWidget();
	_settingsWidget			= new SettingsWidget();
}

ImageViewerPlugin::~ImageViewerPlugin()
{
}

void ImageViewerPlugin::init()
{
	addWidget(_imageDataSetsComboBox);
	addWidget(_imagesComboBox);
	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);
}

void ImageViewerPlugin::dataAdded(const QString name)
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	qDebug() << points.propertyNames();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");

		if (type == "SEQUENCE" || type == "STACK") {
			_imageDataSetsComboBox->addItem(name);
		}
	}
}

void ImageViewerPlugin::dataChanged(const QString name)
{
}

void ImageViewerPlugin::dataRemoved(const QString name)
{
	const auto index = _imageDataSetsComboBox->findText(name);

	if (index >= 0)
		_imageDataSetsComboBox->removeItem(index);
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