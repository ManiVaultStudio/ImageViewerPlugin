#include "ImageDatasets.h"

#include <QDebug>

ImageDatasets::ImageDatasets() :
	QObject(),
	_datasets(),
	_currentDatasetName()
{
}

ImageDatasets::~ImageDatasets() = default;

void ImageDatasets::add(const QString& name, const QSharedPointer<ImageDataset>& dataset)
{
	if (_datasets.contains(name))
		throw std::exception(QString("Dataset %1 already exists").arg(name).toLatin1());

	const auto previousDatasetName = _currentDatasetName;

	_datasets.insert(name, dataset);

	qDebug() << name << "added";

	emit namesChanged(_datasets.keys());

	setCurrentDatasetName(name);
}

void ImageDatasets::setCurrentDatasetName(const QString& currentDatasetName)
{
	if (currentDatasetName == _currentDatasetName)
		return;

	qDebug() << "Set current dataset name" << currentDatasetName;

	const auto previousDatasetName = _currentDatasetName;

	_currentDatasetName = currentDatasetName;

	//datasetByName(currentDatasetName)->activate();

	emit currentDatasetNameChanged(previousDatasetName, _currentDatasetName);
	emit currentDatasetChanged(datasetByName(previousDatasetName), datasetByName(currentDatasetName));
}

ImageDataset* ImageDatasets::currentDataset()
{
	return datasetByName(_currentDatasetName);
}

ImageDataset* ImageDatasets::datasetByName(const QString& datasetName)
{
	if (!_datasets.contains(datasetName))
		return nullptr;

	return _datasets.value(datasetName).get();
}