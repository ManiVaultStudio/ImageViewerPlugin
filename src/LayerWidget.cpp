#include "LayerWidget.h"
#include "LayersModel.h"
#include "LayerNode.h"
#include "ImageViewerPlugin.h"

#include "ui_LayerWidget.h"

#include <QItemSelectionModel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDebug>

#include <set>

LayerWidget::LayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::LayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);

	_ui->generalGroupBox->setVisible(false);
}

void LayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;
	_layersModel = &_imageViewerPlugin->layersModel();

	_ui->pointsLayerWidget->initialize(_imageViewerPlugin);
	_ui->imagesLayerWidget->initialize(_imageViewerPlugin);
	_ui->clustersLayerWidget->initialize(_imageViewerPlugin);
	_ui->selectionLayerWidget->initialize(_imageViewerPlugin);

	_ui->settingsStackedWidget->setVisible(false);

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int state) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();
		
		if (selectedRows.count() == 1) {
			_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(LayerNode::Column::Name)), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->layerNameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(LayerNode::Column::Name)), text);
		}
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(LayerNode::Column::Opacity)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(LayerNode::Column::Opacity)), static_cast<float>(value) / static_cast<float>(range));
		}
	});

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &LayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else
			updateData(selected.indexes().first(), selected.indexes().last());

		_ui->settingsStackedWidget->setVisible(selectedRows.count() == 1);

		if (!selectedRows.isEmpty()) {
			const auto type = selectedRows.first().siblingAtColumn(ult(LayerNode::Column::Type)).data(Qt::EditRole).toInt();
			_ui->settingsStackedWidget->setCurrentIndex(type);
		}
	});
}

void LayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();

	if (noSelectedRows != 1)
		return;

	if (selectedRows.first().row() != topLeft.row())
		return;

	const auto enabled = _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

	_ui->generalGroupBox->setVisible(noSelectedRows == 1);
	_ui->generalGroupBox->setEnabled(noSelectedRows == 1);

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index	= topLeft.siblingAtColumn(column);

		auto validSelection	= false;
		auto flags			= 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection	= true;
			flags			= _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Flags)), Qt::EditRole).toInt();
		}
		
		const auto mightEdit = validSelection && enabled;

		if (column == ult(LayerNode::Column::Name)) {
			_ui->layerEnabledCheckBox->setEnabled(noSelectedRows == 1);
			_ui->layerEnabledCheckBox->blockSignals(true);
			_ui->layerEnabledCheckBox->setChecked(enabled);
			_ui->layerEnabledCheckBox->blockSignals(false);

			const auto nameFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(LayerNode::Column::Name)));
			const auto name			= validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Name)), Qt::EditRole).toString() : "";

			_ui->layerNameLabel->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);
			_ui->layerNameLineEdit->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);

			if (name != _ui->layerNameLineEdit->text()) {
				_ui->layerNameLineEdit->blockSignals(true);
				_ui->layerNameLineEdit->setText(name);
				_ui->layerNameLineEdit->blockSignals(false);
			}
		}

		const auto opacityFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(LayerNode::Column::Opacity)));

		_ui->layerOpacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

		if (column == ult(LayerNode::Column::Opacity)) {
			const auto opacity = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Opacity)), Qt::EditRole).toFloat() : 1.0f;

			_ui->layerOpacityDoubleSpinBox->blockSignals(true);
			_ui->layerOpacityDoubleSpinBox->setValue(100.0f * opacity);
			_ui->layerOpacityDoubleSpinBox->blockSignals(false);

			_ui->layerOpacityHorizontalSlider->blockSignals(true);
			_ui->layerOpacityHorizontalSlider->setValue(100.0f * opacity);
			_ui->layerOpacityHorizontalSlider->blockSignals(false);
		}
	}
}