#include "LayerWidget.h"
#include "LayersModel.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

#include "ui_LayerWidget.h"

#include <QItemSelectionModel>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QDebug>

#include <set>

LayerWidget::LayerWidget(QWidget* parent) :
	QWidget(parent),
	_ui{ std::make_unique<Ui::LayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);

	_ui->commonGroupBox->setVisible(false);
	_ui->navigationGroupBox->setVisible(false);
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

	QObject::connect(_ui->enabledCheckBox, &QCheckBox::stateChanged, [this](int state) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();
		
		if (selectedRows.count() == 1) {
			_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(Layer::Column::Name)), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->nameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Name)), text);
		}
	});

	QObject::connect(_ui->opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->opacityDoubleSpinBox->maximum() - _ui->opacityDoubleSpinBox->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->opacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->opacityHorizontalSlider->maximum() - _ui->opacityHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), static_cast<float>(value) / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->scaleDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Scale)), static_cast<float>(value) / 100.0f);
		}
	});

	QObject::connect(_ui->scaleHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->scaleHorizontalSlider->maximum() - _ui->scaleHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Scale)), static_cast<float>(value) / 100.0f);
		}
	});

	QObject::connect(_ui->resetPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), 1.0f);
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Scale)), 1.0f);
		}
	});

	QObject::connect(_ui->zoomExtentsPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->getLayer(selectedRows.first())->zoomExtents();
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
			const auto type = selectedRows.first().siblingAtColumn(ult(Layer::Column::Type)).data(Qt::EditRole).toInt();
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

	const auto enabled = _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

	_ui->commonGroupBox->setVisible(noSelectedRows == 1);
	_ui->commonGroupBox->setEnabled(noSelectedRows == 1);
	_ui->navigationGroupBox->setVisible(noSelectedRows == 1);

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index	= topLeft.siblingAtColumn(column);

		auto validSelection	= false;
		auto flags			= 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection	= true;
			flags			= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Flags)), Qt::EditRole).toInt();
		}
		
		const auto mightEdit = validSelection && enabled;

		if (column == ult(Layer::Column::Name)) {
			_ui->enabledCheckBox->setEnabled(noSelectedRows == 1);
			_ui->enabledCheckBox->blockSignals(true);
			_ui->enabledCheckBox->setChecked(enabled);
			_ui->enabledCheckBox->blockSignals(false);

			const auto nameFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::Name)));
			const auto name			= validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::EditRole).toString() : "";

			_ui->nameLabel->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);
			_ui->nameLineEdit->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);

			if (name != _ui->nameLineEdit->text()) {
				_ui->nameLineEdit->blockSignals(true);
				_ui->nameLineEdit->setText(name);
				_ui->nameLineEdit->blockSignals(false);
			}
		}

		const auto opacityFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)));

		_ui->opacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->opacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->opacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

		if (column == ult(Layer::Column::Opacity)) {
			const auto opacity = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)), Qt::EditRole).toFloat() : 1.0f;

			_ui->opacityDoubleSpinBox->blockSignals(true);
			_ui->opacityDoubleSpinBox->setValue(100.0f * opacity);
			_ui->opacityDoubleSpinBox->blockSignals(false);

			_ui->opacityHorizontalSlider->blockSignals(true);
			_ui->opacityHorizontalSlider->setValue(100.0f * opacity);
			_ui->opacityHorizontalSlider->blockSignals(false);
		}

		const auto scaleFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)));

		_ui->scaleLabel->setEnabled(mightEdit && scaleFlags & Qt::ItemIsEditable);
		_ui->scaleDoubleSpinBox->setEnabled(mightEdit && scaleFlags & Qt::ItemIsEditable);
		_ui->scaleHorizontalSlider->setEnabled(mightEdit && scaleFlags & Qt::ItemIsEditable);

		if (column == ult(Layer::Column::Scale)) {
			const auto scale = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Scale)), Qt::EditRole).toFloat() : 1.0f;

			_ui->scaleDoubleSpinBox->blockSignals(true);
			_ui->scaleDoubleSpinBox->setValue(100.0f * scale);
			_ui->scaleDoubleSpinBox->blockSignals(false);

			_ui->scaleHorizontalSlider->blockSignals(true);
			_ui->scaleHorizontalSlider->setValue(100.0f * scale);
			_ui->scaleHorizontalSlider->blockSignals(false);
		}

		if (column == ult(Layer::Column::Opacity) || column == ult(Layer::Column::Scale)) {
			const auto opacity	= validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)), Qt::EditRole).toFloat() : 1.0f;
			const auto scale	= validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Scale)), Qt::EditRole).toFloat() : 1.0f;

			_ui->resetPushButton->setEnabled(opacity != 1.0f || scale != 1.0f);
		}
	}
}