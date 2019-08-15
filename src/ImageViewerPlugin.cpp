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

PointsPlugin& ImageViewerPlugin::pointsData() const
{
	const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(_currentDataSetName));

	return set.getData();
}

std::vector<unsigned int> ImageViewerPlugin::selection() const
{
	const IndexSet& selection = dynamic_cast<const IndexSet&>(pointsData().getSelection());

	return selection.indices;
}

bool ImageViewerPlugin::hasSelection() const
{
	return selection().size() > 0;
}

QString ImageViewerPlugin::imageCollectionType() const
{
	if (_currentDataSetName.isEmpty())
		return "";

	PointsPlugin& points = pointsData();

	if (points.hasProperty("type")) {
		return points.getProperty("type").toString();
	}

	return "";
}

QStringList ImageViewerPlugin::dimensionNames() const
{
	PointsPlugin& points = pointsData();

	const auto noDimensions = points.dimNames.size();

	auto dimensionNames = QStringList();

	dimensionNames.reserve(noDimensions);

	for (size_t i = 0, l = noDimensions; i < l; ++i)
		dimensionNames << points.dimNames[i];

	return dimensionNames;
}

int ImageViewerPlugin::noImages() const
{
	PointsPlugin& points = pointsData();

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
	qDebug() << "Set current data set name: " << currentDataSetName;

	_currentDataSetName = currentDataSetName;

	emit currentDataSetNameChanged(_currentDataSetName);
}

bool ImageViewerPlugin::averageImages() const
{
	return _averageImages;
}

void ImageViewerPlugin::setAverageImages(const bool& averageImages)
{
	qDebug() << "Set average images: " << averageImages;

	_averageImages = averageImages;

	emit averageImagesChanged(_averageImages);
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
	setAverageImages(hasSelection() && imageCollectionType() == "SEQUENCE");

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