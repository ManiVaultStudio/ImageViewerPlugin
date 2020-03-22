#include "ImagesLayerWidget.h"
#include "LayersModel.h"

#include "ui_ImagesLayerWidget.h"

#include <QStringListModel>
#include <QDebug>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() }
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;
}

void ImagesLayerWidget::updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	const auto selectedRows = _layersModel->selectionModel().selectedRows();

	if (selectedRows.size() != 1)
		return;

	const auto filteredImageNames = _layersModel->data(topLeft.row(), LayerColumn::FilteredImageNames, Qt::EditRole).toStringList();

	_ui->imagesComboBox->setModel(new QStringListModel(filteredImageNames));
}