#include "LayerWidget.h"

#include "ui_LayerWidget.h"

#include <QDebug>

LayerWidget::LayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::LayerWidget>() }
{
	_ui->setupUi(this);
}

void LayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
}