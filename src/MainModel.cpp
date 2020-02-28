#include "ImageViewerPlugin.h"
#include "DatasetsModel.h"
#include "LayersModel.h"
#include "MainModel.h"

#include <QItemSelectionModel>
#include <QDebug>

MainModel::MainModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractListModel(imageViewerPlugin),
	_datasets(),
	_currentDatasetID(),
	_datasetsModel(new DatasetsModel(this)),
	_layersModel(new LayersModel(this)),
	_selectionModel(new QItemSelectionModel(_datasetsModel))
{
	QObject::connect(_selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		qDebug() << current;
	});
}

MainModel::~MainModel() = default;

QVariant MainModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() != 0 ||index.column() != 0)
		return QVariant();

	return _currentDatasetID;
}

Qt::ItemFlags MainModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool MainModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
	if (index.isValid() && role == Qt::EditRole) {
		
		_currentDatasetID = value.toInt();

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

void MainModel::addDataset(const Dataset& dataset)
{
	_datasetsModel->add(dataset);
}