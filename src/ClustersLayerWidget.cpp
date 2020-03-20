#include "ClustersLayerWidget.h"

#include "ui_ClustersLayerWidget.h"

#include <QDebug>

ClustersLayerWidget::ClustersLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::ClustersLayerWidget>() }
{
	_ui->setupUi(this);
}

void ClustersLayerWidget::initialize(LayersModel* layersModel)
{
}

void ClustersLayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
}