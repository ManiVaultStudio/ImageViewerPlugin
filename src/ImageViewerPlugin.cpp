#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsWidget.h"
#include "ImageDataset.h"
#include "DatasetsModel.h"
#include "Renderer.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QDebug>
#include <QFileInfo>

#include "IndexSet.h"

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() : 
	ViewPlugin("Image Viewer"),
	_imageViewerWidget(),
	_settingsWidget(),
	_mainModel(this)
{
	qRegisterMetaType<std::shared_ptr<QImage>>("std::shared_ptr<QImage>");

	//setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	_imageViewerWidget	= new ImageViewerWidget(&_mainModel);
	_settingsWidget		= new SettingsWidget(this, &_mainModel);
}

void ImageViewerPlugin::init()
{
	auto layout = new QHBoxLayout();
	
	layout->setMargin(0);
	layout->setSpacing(0);
	
	setMainLayout(layout);

	addWidget(_imageViewerWidget);
	addWidget(_settingsWidget);

	layout->setStretchFactor(_imageViewerWidget, 1);
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

	auto imagesDataset = _core->requestData<Images>(dataset);

	auto imageDataset = new ImageDataset(this);

	imageDataset->_name				= dataset;
	imageDataset->_type				= static_cast<int>(imagesDataset.imageCollectionType());
	imageDataset->_noImages			= imagesDataset.noImages();
	imageDataset->_size				= imagesDataset.imageSize();
	imageDataset->_noPoints			= imagesDataset.points()->getNumPoints();
	imageDataset->_noDimensions		= imagesDataset.points()->getNumDimensions();
	imageDataset->_currentImage		= 0;
	imageDataset->_currentDimension	= 0;
	imageDataset->_averageImages		= false;
	imageDataset->_imageFilePaths	= QStringList();

	for (const auto& imageFilePath : imagesDataset.imageFilePaths()) {
		imageDataset->_imageFilePaths << imageFilePath;
	}

	switch (imagesDataset.imageCollectionType())
	{
		case ImageCollectionType::Sequence:
		{
			for (const auto& imageFilePath : imagesDataset.imageFilePaths()) {
				imageDataset->_imageNames << QFileInfo(imageFilePath).fileName();
			}
			break;
		}

		case ImageCollectionType::Stack:
		{
			for (const auto& dimensionName : imagesDataset.dimensionNames()) {
				imageDataset->_dimensionNames << dimensionName;
			}
			break;
		}

		default:
			break;
	}

	imageDataset->_averageImages	= false;

	_mainModel.addDataset(imageDataset);
}

void ImageViewerPlugin::dataChanged(const QString dataset)
{
	qDebug() << "Data changed" << dataset;
}

void ImageViewerPlugin::dataRemoved(const QString dataset)
{
	qDebug() << "Data removed" << dataset;
}

void ImageViewerPlugin::selectionChanged(const QString dataset)
{
	//emit _imageDatasetsModel.currentDataset()->setSelectionChanged();
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