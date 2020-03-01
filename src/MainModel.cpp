#include "MainModel.h"
#include "ImageViewerPlugin.h"
#include "DatasetsModel.h"

#include <QDebug>

MainModel::MainModel(ImageViewerPlugin* imageViewerPlugin) :
	QObject(imageViewerPlugin),
	_datasets(),
	_currentDatasetID(),
	_datasetsModel(new DatasetsModel(this))
{
}

MainModel::~MainModel() = default;

void MainModel::addDataset(const ImageDataset& dataset)
{
	_datasetsModel->add(dataset);
}