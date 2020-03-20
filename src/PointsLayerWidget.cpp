#include "PointsLayerWidget.h"

#include "ui_PointsLayerWidget.h"

#include <QDebug>

PointsLayerWidget::PointsLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::PointsLayerWidget>() }
{
	_ui->setupUi(this);
}

void PointsLayerWidget::initialize(LayersModel* layersModel)
{
}

void PointsLayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
}