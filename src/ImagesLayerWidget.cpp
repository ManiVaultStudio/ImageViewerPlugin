#include "ImagesLayerWidget.h"
#include "LayersModel.h"
#include "ImagesLayer.h"

#include "ui_ImagesLayerWidget.h"

#include <QItemSelectionModel>
#include <QStringListModel>
#include <QDebug>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	ModelWidget(parent, static_cast<int>(ImagesLayer::Column::End)),
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() }
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::initialize(LayersModel* layersModel)
{
	ModelWidget::initialize(layersModel);

	/*
	QObject::connect(_->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		qDebug() << "Current" << current;
		qDebug() << "Index" << _layersModel->index(0, 0, current);
		setIndex(_layersModel->index(0, 0, current));
	});
	*/
}

void ImagesLayerWidget::updateData(const QModelIndex& index)
{
	qDebug() << "Update data" << index.data(Qt::DisplayRole);
}

bool ImagesLayerWidget::shouldUpdate(const QModelIndex& index) const
{
	/*
	qDebug() << "index.parent()" << index.parent();
	return true;
	const auto type = static_cast<LayerItem::Type>(_layersModel->data(index.parent().row(), static_cast<int>(LayerItem::Column::Type), Qt::EditRole).toInt());

	return type == LayerItem::Type::Images;
	*/

	return true;
}