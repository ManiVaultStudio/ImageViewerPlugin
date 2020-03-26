#include "ModelWidget.h"
#include "LayersModel.h"

#include <QItemSelectionModel>
#include <QStringListModel>
#include <QDebug>

ModelWidget::ModelWidget(QWidget* parent, const int& noColumns) :
	QWidget(parent),
	_layersModel(nullptr),
	_index(),
	_noColumns(noColumns)
{
}

void ModelWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ModelWidget::onDataChanged);
}

void ModelWidget::setIndex(const QModelIndex& index)
{
	_index = index;
	
	qDebug() << "setIndex()" << _index << index << _index.parent();

	for (int column = 0; column < _noColumns; column++) {
		const auto siblingIndex = _index.siblingAtColumn(column);
		
		if (shouldUpdate(siblingIndex))
			updateData(siblingIndex);
	}
}

void ModelWidget::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	if (topLeft.parent() != _index.parent())
		return;

	if (topLeft.row() != _index.row())
		return;

	if (!shouldUpdate(_index))
		return;

	for (int columnId = topLeft.column(); columnId <= bottomRight.column(); columnId++) {
		updateData(_layersModel->index(topLeft.row(), columnId, topLeft.parent()));
	}
}