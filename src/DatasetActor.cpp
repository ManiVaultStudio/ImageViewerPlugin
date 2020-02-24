#include "DatasetActor.h"
#include "LayerProp.h"
#include "Renderer.h"
#include "Dataset.h"

#include <QOpenGLTexture>
#include <QDebug>

DatasetActor::DatasetActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_dataset(nullptr)
{
}

void DatasetActor::setDataset(Dataset* dataset)
{
	if (dataset == _dataset)
		return;
	
	auto previousDataset = _dataset;

	_dataset = dataset;

	// Disconnect from dataset
	if (previousDataset != nullptr) {
		disconnectFromDataset(previousDataset);
	}

	// Connect to dataset
	if (_dataset != nullptr) {
		connectToDataset(dataset);
	}

	/*
	qDebug() << name() << "set layer";

	this->propByName<ImageProp>("ImageProp")->setImage(dataset->image());
	this->propByName<ImageProp>("ImageProp")->setDisplayRange(dataset->windowLevel().displayRange());

	connect(_dataset, &Layer::imageChanged, this, [&](const QImage& image) {
		this->propByName<ImageProp>("ImageProp")->setImage(image);
	});

	connect(&_dataset->windowLevel(), &WindowLevel::displayRangeChanged, this, [&](QPair<float, float> displayRange) {
		this->propByName<ImageProp>("ImageProp")->setDisplayRange(displayRange);
	});
	*/
}

void DatasetActor::connectToDataset(Dataset* dataset)
{
	if (dataset == nullptr)
		return;

	qDebug() << "Connecting to dataset" << dataset->name();

	QObject::connect(dataset, &Dataset::addLayer, this, &DatasetActor::onAddLayer);
	QObject::connect(dataset, &Dataset::removeLayer, this, &DatasetActor::onRemoveLayer);
}

void DatasetActor::disconnectFromDataset(Dataset* dataset)
{
	if (dataset == nullptr)
		return;

	qDebug() << "Disconnecting from dataset" << dataset->name();

	QObject::disconnect(dataset, &Dataset::addLayer, this, &DatasetActor::onAddLayer);
	QObject::disconnect(dataset, &Dataset::removeLayer, this, &DatasetActor::onRemoveLayer);
}

void DatasetActor::onAddLayer(ImageLayer* layer)
{

}

void DatasetActor::onRemoveLayer(ImageLayer* layer)
{

}

void DatasetActor::addLayerProp(ImageLayer* layer)
{

}

void DatasetActor::removeLayerProp(ImageLayer* layer)
{

}
