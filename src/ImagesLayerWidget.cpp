#include "ImagesLayerWidget.h"

#include "ui_ImagesLayerWidget.h"

#include <QDebug>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() }
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::initialize(LayersModel* layersModel)
{
}

void ImagesLayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
}