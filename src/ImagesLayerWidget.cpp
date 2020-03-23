#include "ImagesLayerWidget.h"
#include "LayersModel.h"
#include "ImagesLayer.h"

#include "ui_ImagesLayerWidget.h"

#include <QStringListModel>
#include <QDebug>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() }
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::setImagesLayer(ImagesLayer* imagesLayer)
{
	_imagesLayer = imagesLayer;
}

void ImagesLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	auto columnIds = QSet<int>();

	for (int columnId = topLeft.column(); columnId <= bottomRight.column(); columnId++) {
		columnIds.insert(columnId);
	}

	/*
	if (columnIds.contains(static_cast<int>(ImagesLayer::Column::FilteredImageNames))) {
		const auto filteredImageNames = _layersModel->data(topLeft.row(), static_cast<int>(ImagesLayer::Column::FilteredImageNames), Qt::EditRole).toStringList();

		_ui->imagesComboBox->setModel(new QStringListModel(filteredImageNames));
	}
	*/
}