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

Indices ImageViewerPlugin::selection() const
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

QSize ImageViewerPlugin::imageSize() const
{
	PointsPlugin& points = pointsData();

	if (points.hasProperty("imageSize")) {
		return points.getProperty("imageSize").toSize();
	}

	return QSize();
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

	updateDisplayImageIds();

	emit averageImagesChanged(_averageImages);
}

Index ImageViewerPlugin::currentImageId() const
{
	return _currentImageId;
}

void ImageViewerPlugin::setCurrentImageId(const int& currentImageId)
{
	if (currentImageId < 0)
		return;

	qDebug() << "Set current image index to:" << currentImageId;

	_currentImageId = currentImageId;

	updateDisplayImageIds();
}

Indices ImageViewerPlugin::displayImageIds() const
{
	return _displayImageIds;
}

void ImageViewerPlugin::updateDisplayImageIds()
{
	// qDebug() << "Update display image IDs";

	const auto type = imageCollectionType();

	if (_averageImages) {
		if (type == "SEQUENCE") {
			if (hasSelection()) {
				_displayImageIds = selection();
			} else {
				_displayImageIds.resize(noImages());
				std::iota(std::begin(_displayImageIds), std::end(_displayImageIds), 0);
			}
		}
		
		if (type == "STACK") {
			_displayImageIds.resize(noImages());
			std::iota(std::begin(_displayImageIds), std::end(_displayImageIds), 0);
		}
	} else {
		_displayImageIds = Indices({ _currentImageId });
	}

	emit displayImageIdsChanged();

	/*
	auto imageIds = QStringList();

	for (auto displayImageId : _displayImageIds) {
		imageIds << QString::number(displayImageId);
	}
	
	qDebug() << imageIds.size();
	*/
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
	// setAverageImages(hasSelection() && imageCollectionType() == "SEQUENCE");

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