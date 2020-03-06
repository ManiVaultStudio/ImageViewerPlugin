#include "ImageDatasetActor.h"
#include "ImageLayerProp.h"
#include "ImageDataset.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QOpenGLTexture>
#include <QDebug>

ImageDatasetActor::ImageDatasetActor(Renderer* renderer, const QString& name, LayersModel* layersModel, const bool& visible /*= true*/) :
	Actor(renderer, name, visible),
	_layersModel(layersModel)
{
	/*
	if (_imageDataset == nullptr)
		throw std::exception("Image dataset is null");

	qDebug() << "Connecting to dataset" << _imageDataset->name();

	for (const auto& name : _imageDataset->imageLayers().keys()) {
		addLayerProp(name);
	}

	QObject::connect(_imageDataset, &ImageDataset::layerAdded, this, &ImageDatasetActor::addLayerProp);
	QObject::connect(_imageDataset, &ImageDataset::layerRemoved, this, &ImageDatasetActor::removeLayerProp);
	*/

	for (int row = 0; row < _layersModel->rowCount(); row++) {
		const auto layerName = _layersModel->data(row, LayersModel::Columns::Name);
		qDebug() << layerName;
		//addProp<ImageLayerProp>(this, layerName, _imageDataset->imageLayerByName(layerName));
	}
}

void ImageDatasetActor::addLayerProp(const QString& layerName)
{
	/*
	try
	{
		qDebug() << "Add layer prop" << layerName;

		addProp<ImageLayerProp>(this, layerName, _imageDataset->imageLayerByName(layerName));
	}
	catch (std::exception& e)
	{
		qDebug() << "Unable to add layer prop:" << e.what();
	}
	*/
}

void ImageDatasetActor::removeLayerProp(const QString& layerName)
{
	try
	{
		qDebug() << "Remove layer prop" << layerName;

		removeProp(layerName);
	}
	catch (std::exception& e)
	{
		qDebug() << "Unable to remove layer prop:" << e.what();
	}
}