#include "Datasets.h"

#include <QDebug>

Datasets::Datasets() :
	QObject(),
	_datasets(),
	_currentDatasetName()
{
}

Datasets::~Datasets() = default;

void Datasets::add(const QString& name, const QSharedPointer<Dataset>& dataset)
{
	if (_datasets.contains(name))
		throw std::exception(QString("Dataset %1 already exists").arg(name).toLatin1());

	const auto previousDatasetName = _currentDatasetName;

	_datasets.insert(name, dataset);

	qDebug() << name << "added";

	emit namesChanged(_datasets.keys());

	setCurrentDatasetName(name);
}

void Datasets::setCurrentDatasetName(const QString& currentDatasetName)
{
	if (currentDatasetName == _currentDatasetName)
		return;

	qDebug() << "Set current dataset name" << currentDatasetName;

	const auto previousDatasetName = _currentDatasetName;

	_currentDatasetName = currentDatasetName;

	emit currentDatasetNameChanged(previousDatasetName, _currentDatasetName);
	emit currentDatasetChanged(datasetByName(previousDatasetName), datasetByName(currentDatasetName));

	emit currentDataset()->selectionChanged();
}

Dataset* Datasets::currentDataset()
{
	return datasetByName(_currentDatasetName);
}

Dataset* Datasets::datasetByName(const QString& datasetName)
{
	if (!_datasets.contains(datasetName))
		return nullptr;

	return _datasets.value(datasetName).get();
}