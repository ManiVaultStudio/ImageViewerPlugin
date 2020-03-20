#include "SelectionLayerWidget.h"

#include "ui_SelectionLayerWidget.h"

#include <QDebug>

SelectionLayerWidget::SelectionLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::SelectionLayerWidget>() }
{
	_ui->setupUi(this);
}

void SelectionLayerWidget::initialize(LayersModel* layersModel)
{
}

void SelectionLayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
}