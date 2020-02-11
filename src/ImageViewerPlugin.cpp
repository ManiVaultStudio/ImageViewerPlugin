#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"
#include "ColorImageActor.h"
#include "Renderer.h"

#include <QDebug>
#include <QFileInfo>

#include "IndexSet.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(),
	_settingsWidget()
{
	qRegisterMetaType<std::shared_ptr<QImage>>("std::shared_ptr<QImage>");

	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(this);
	_settingsWidget		= new SettingsWidget(&_datasets);
}

void ImageViewerPlugin::init()
{
	auto layout = new QVBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);

	setMainLayout(layout);

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);
}

ImageViewerWidget* ImageViewerPlugin::imageViewerWidget()
{
	return _imageViewerWidget;
}

void ImageViewerPlugin::updateWindowTitle()
{
	QStringList properties;

	//properties << QString("dataset=%1").arg(_currentDatasetName);
	//properties << QString("image=%1").arg(_imageNames.size() > 0 ? _imageNames[_currentImageId] : "");
	//properties << QString("dimension=%1").arg(_dimensionNames.size() > 0 ? _dimensionNames[_currentDimensionId] : "");
	
	//auto imageQuad = _imageViewerWidget->renderer()->imageQuad();

	//const auto size = imageQuad->size();

	/*
	properties << QString("width=%1").arg(QString::number(size.width()));
	properties << QString("height=%1").arg(QString::number(size.height()));
	properties << QString("window=%1").arg(QString::number(imageQuad->windowNormalized(), 'f', 2));
	properties << QString("level=%1").arg(QString::number(imageQuad->levelNormalized(), 'f', 2));
	properties << QString("imageMin=%1").arg(QString::number(imageQuad->imageMin()));
	properties << QString("imageMax=%1").arg(QString::number(imageQuad->imageMax()));
	properties << QString("noSelectedPixels=%1").arg(QString::number(selection().size()));
	properties << QString("noPixels=%1").arg(QString::number(size.width() * size.height()));
	*/

	setWindowTitle(QString("%1").arg(properties.join(", ")));
}

void ImageViewerPlugin::dataAdded(const QString dataset)
{
	qDebug() << "Data added" << dataset;

	_datasets.add(dataset, QSharedPointer<Dataset>::create(dataset, &_core->requestData<Images>(dataset)));
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
	qDebug() << "Data changed" << dataset;
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
	qDebug() << "Data removed" << dataset;
	
	// TODO: _datasetNames.removeAt(_datasetNames.indexOf(dataset));
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	emit _datasets.currentDataset()->selectionChanged();
}

hdps::DataTypes ImageViewerPlugin::supportedDataTypes() const
{
	hdps::DataTypes supportedTypes;
	supportedTypes.append(ImageType);
	return supportedTypes;
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}