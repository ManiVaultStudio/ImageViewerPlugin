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

	_datasets.insert(name, dataset);

	qDebug() << name << "added";
}

void Datasets::setCurrentDatasetName(const QString& currentDatasetName)
{
	if (currentDatasetName == _currentDatasetName)
		return;

	qDebug() << "Set current dataset name" << currentDatasetName;

	_currentDatasetName = currentDatasetName;

	emit currentDatasetNameChanged(_currentDatasetName);
}