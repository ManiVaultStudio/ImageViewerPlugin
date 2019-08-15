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
	_imageViewerWidget	= new ImageViewerWidget();
	_settingsWidget		= new SettingsWidget(this);
}

ImageViewerPlugin::~ImageViewerPlugin()
{
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	setLayout(layout);
}

QString ImageViewerPlugin::dataSetType(const QString& name) const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		return points.getProperty("type").toString();
	}

	return QString();
}

QStringList ImageViewerPlugin::dataSetDimensionNames(const QString & name) const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0, l = noDimensions; i < l; ++i)
		dimensionNames << points.dimNames[i];

	return dimensionNames;
}

void ImageViewerPlugin::dataAdded(const QString name)
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	PointsPlugin& points = set.getData();

	if (points.hasProperty("type")) {
		const auto type = points.getProperty("type");

		if (type == "SEQUENCE" || type == "STACK") {
			_settingsWidget->addDataSet(name);
		}
	}
}

void ImageViewerPlugin::dataChanged(const QString name)
{
}

void ImageViewerPlugin::dataRemoved(const QString name)
{
	_settingsWidget->removeDataSet(name);
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