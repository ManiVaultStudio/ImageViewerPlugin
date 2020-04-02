#include "SelectionLayerWidget.h"
#include "LayersModel.h"
#include "SelectionLayer.h"

#include "ui_SelectionLayerWidget.h"

#include <QDebug>

SelectionLayerWidget::SelectionLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::SelectionLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void SelectionLayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &SelectionLayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else
			updateData(selected.indexes().first(), selected.indexes().last());
	});
}

void SelectionLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto enabled			= _layersModel->data(topLeft.row(), ult(Layer::Column::Enabled), Qt::EditRole).toBool();

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index = _layersModel->index(topLeft.row(), column);

		auto validSelection = false;
		auto flags = 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection = true;
			flags = _layersModel->data(topLeft.row(), ult(Layer::Column::Flags), Qt::EditRole).toInt();
		}

		const auto mightEdit = validSelection && enabled;

		/*
		_ui->groupBox->setEnabled(enabled);
		
		const auto opacityFlags = _layersModel->flags(topLeft.row(), ult(Layer::Column::Opacity));

		_ui->layerOpacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

		if (column == ult(Layer::Column::Opacity)) {
			const auto opacity = validSelection ? _layersModel->data(topLeft.row(), ult(Layer::Column::Opacity), Qt::EditRole).toFloat() : 1.0f;

			_ui->layerOpacityDoubleSpinBox->blockSignals(true);
			_ui->layerOpacityDoubleSpinBox->setValue(100.0f * opacity);
			_ui->layerOpacityDoubleSpinBox->blockSignals(false);

			_ui->layerOpacityHorizontalSlider->blockSignals(true);
			_ui->layerOpacityHorizontalSlider->setValue(100.0f * opacity);
			_ui->layerOpacityHorizontalSlider->blockSignals(false);
		}
		*/
	}
}