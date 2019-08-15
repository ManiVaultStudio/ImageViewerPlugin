#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"

#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QImageReader>
#include <vector>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(nullptr),
	_settingsWidget(nullptr),
	_currentDataSetName("")
{
	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(this);

	//connect(_settingsWidget, &SettingsWidget::currentImageChanged, _imageViewerWidget, &ImageViewerWidget::onCurrentImageChanged);
	//connect(this, &ImageViewerPlugin::selectedPointsChanged, _imageViewerWidget, &ImageViewerWidget::onSelectedPointsChanged);
	//connect(this, &ImageViewerPlugin::selectedPointsChanged, _settingsWidget, &SettingsWidget::onSelectedPointsChanged);
	
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

PointsPlugin& ImageViewerPlugin::pointsData(const QString& name) const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(name));

	return set.getData();
}

std::vector<unsigned int> ImageViewerPlugin::selection(const QString& name) const
{
	const IndexSet& selection = dynamic_cast<const IndexSet&>(pointsData(name).getSelection());

	return selection.indices;
}

bool ImageViewerPlugin::hasSelection(const QString& name) const
{
	return selection(name).size() > 0;
}

QString ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDataSetName.isEmpty())
		return "";

	PointsPlugin& points = pointsData(_currentDataSetName);

	if (points.hasProperty("type")) {
		return points.getProperty("type").toString();
	}

	return "";
}

QStringList ImageViewerPlugin::dimensionNames(const QString & name) const
{
	PointsPlugin& points = pointsData(name);

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0, l = noDimensions; i < l; ++i)
		dimensionNames << points.dimNames[i];

	return dimensionNames;
}

int ImageViewerPlugin::noImages(const QString & name) const
{
	PointsPlugin& points = pointsData(name);

	if (points.hasProperty("noImages")) {
		return points.getProperty("noImages").toInt();
	}

	return 0;
}

QString ImageViewerPlugin::currentDataSetName() const
{
	return _currentDataSetName;
}

void ImageViewerPlugin::setCurrentDataSetName(const QString& currentDataSetName)
{
	_currentDataSetName = currentDataSetName;

	emit currentDataSetNameChanged(_currentDataSetName);
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
	/*
	const auto currentDataSetName = _settingsWidget->currentDataSetName();

	if (currentDataSetName.isEmpty())
		return;

	PointsPlugin& points = pointsData(currentDataSetName);

	if (points.isDerivedData())
	{
		if (dataName != points.getSourceData().getName())
			return;
	}
	else
	{
		if (dataName != points.getName())
			return;
	}
	*/

	emit selectedPointsChanged();
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