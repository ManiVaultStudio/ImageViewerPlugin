#include "ImageDatasetActor.h"
#include "ImageLayerProp.h"
#include "Renderer.h"
#include "ImageDataset.h"

#include <QOpenGLTexture>
#include <QDebug>

ImageDatasetActor::ImageDatasetActor(Renderer* renderer, const QString& name, const bool& visible /*= true*/) :
	Actor(renderer, name, visible),
	_dataset(nullptr)
{
}

void ImageDatasetActor::setDataset(ImageDataset* dataset)
{
	if (dataset == _dataset)
		return;
	
	auto previousDataset = _dataset;

	_dataset = dataset;

	// Disconnect from dataset
	if (previousDataset != nullptr) {
		disconnectFromDataset();
	}

	// Connect to dataset
	if (_dataset != nullptr) {
		connectToDataset();
	}
}

void ImageDatasetActor::connectToDataset()
{
	if (_dataset == nullptr)
		return;

	try
	{
		qDebug() << "Connecting to dataset" << _dataset->name();

		for (const auto& name : _dataset->layerNames()) {
			addLayerProp(name);
		}

		QObject::connect(_dataset, &ImageDataset::layerAdded, this, &ImageDatasetActor::addLayerProp);
		QObject::connect(_dataset, &ImageDataset::layerRemoved, this, &ImageDatasetActor::removeLayerProp);
	}
	catch (const std::exception& e)
	{
		qDebug() << "Unable to connect to" << _dataset->name() << ":" << e.what();
	}
}

void ImageDatasetActor::disconnectFromDataset()
{
	if (_dataset == nullptr)
		return;

	try
	{
		qDebug() << "Disconnecting from dataset" << _dataset->name();

		for (const auto& name : props().keys()) {
			removeLayerProp(name);
		}

		QObject::disconnect(_dataset, &ImageDataset::layerAdded, this, &ImageDatasetActor::addLayerProp);
		QObject::disconnect(_dataset, &ImageDataset::layerRemoved, this, &ImageDatasetActor::removeLayerProp);
	}
	catch (const std::exception& e)
	{
		qDebug() << "Unable to disconnect from" << _dataset->name() << ":" << e.what();
	}
}

void ImageDatasetActor::addLayerProp(const QString& layerName)
{
	try
	{
		qDebug() << "Add layer prop" << layerName;

		addProp<ImageLayerProp>(this, layerName, _dataset->layer(layerName));
	}
	catch (std::exception& e)
	{
		qDebug() << "Unable to add layer prop:" << e.what();
	}
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