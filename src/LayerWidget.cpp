#include "LayerWidget.h"
#include "LayersModel.h"

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
}

void LayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	/*
	_ui->maskCheckBox->setVisible(false);

	QObject::connect(_ui->layerNameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			_layersModel->setData(index.row(), to_underlying(Layer::Column::Name), text);
		}
	});

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int state) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			_layersModel->setData(index.row(), to_underlying(Layer::Column::Enabled), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();

		_layersModel->setData(_layersModel->selectionModel()->currentIndex().row(), to_underlying(Layer::Column::Opacity), value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();

			_layersModel->setData(index.row(), to_underlying(Layer::Column::Opacity), static_cast<float>(value) / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			_layersModel->setData(index.row(), to_underlying(Layer::Column::WindowNormalized), value);
		}
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();
			_layersModel->setData(index.row(), to_underlying(Layer::Column::WindowNormalized), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			_layersModel->setData(index.row(), to_underlying(Layer::Column::LevelNormalized), value);
		}
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _layersModel->selectionModel()->selectedRows()) {
			const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();
			_layersModel->setData(index.row(), to_underlying(Layer::Column::LevelNormalized), value / static_cast<float>(range));
		} 
	});
	*/

	_ui->pointsLayerWidget->initialize(_layersModel);
	_ui->imagesLayerWidget->initialize(_layersModel);
	_ui->clustersLayerWidget->initialize(_layersModel);
	_ui->selectionLayerWidget->initialize(_layersModel);
	

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &LayerWidget::onDataChanged);
}

void LayerWidget::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	/*
	const auto selectedRows			= _layersModel->selectionModel()->selectedRows();
	const auto noSelectedRows		= selectedRows.size();
	const auto singleRowSelection	= noSelectedRows == 1;
	const auto enabled				= _layersModel->data(topLeft.row(), to_underlying(Layer::Column::Enabled), Qt::EditRole).toBool();
	const auto mightEdit			= singleRowSelection && enabled;

	auto columns = QSet<int>();

	for (int c = topLeft.column(); c <= bottomRight.column(); c++) {
		columns.insert(c);
	}

	const auto enabledFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::Enabled));

	_ui->generalGroupBox->setEnabled(enabled);

	if (columns.contains(to_underlying(Layer::Column::Enabled))) {
		_ui->layerEnabledCheckBox->setEnabled(singleRowSelection && enabledFlags & Qt::ItemIsEditable);
		_ui->layerEnabledCheckBox->blockSignals(true);
		_ui->layerEnabledCheckBox->setChecked(singleRowSelection ? enabled : false);
		_ui->layerEnabledCheckBox->blockSignals(false);
	}

	const auto nameFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::Name));

	_ui->layerNameLabel->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);
	_ui->layerNameLineEdit->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);

	if (columns.contains(to_underlying(Layer::Column::Name))) {
		const auto name = singleRowSelection ? _layersModel->data(topLeft.row(), to_underlying(Layer::Column::Name), Qt::EditRole).toString() : "";

		if (name != _ui->layerNameLineEdit->text()) {
			_ui->layerNameLineEdit->blockSignals(true);
			_ui->layerNameLineEdit->setText(name);
			_ui->layerNameLineEdit->blockSignals(false);
		}
	}

	const auto opacityFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::Opacity));

	_ui->layerOpacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

	if (columns.contains(to_underlying(Layer::Column::Opacity))) {
		const auto opacity = _layersModel->data(topLeft.row(), to_underlying(Layer::Column::Opacity), Qt::EditRole).toFloat();

		_ui->layerOpacityDoubleSpinBox->blockSignals(true);
		_ui->layerOpacityDoubleSpinBox->setValue(singleRowSelection ? 100.0f * opacity : 100.0f);
		_ui->layerOpacityDoubleSpinBox->blockSignals(false);

		_ui->layerOpacityHorizontalSlider->blockSignals(true);
		_ui->layerOpacityHorizontalSlider->setValue(singleRowSelection ? 100.0f * opacity : 100.0f);
		_ui->layerOpacityHorizontalSlider->blockSignals(false);
	}

	const auto windowFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::WindowNormalized));

	_ui->layerWindowLabel->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowDoubleSpinBox->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowHorizontalSlider->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);

	if (columns.contains(to_underlying(Layer::Column::WindowNormalized))) {
		const auto window = _layersModel->data(topLeft.row(), to_underlying(Layer::Column::WindowNormalized), Qt::EditRole).toFloat();

		_ui->layerWindowDoubleSpinBox->blockSignals(true);
		_ui->layerWindowDoubleSpinBox->setValue(singleRowSelection ? window : 1.0f);
		_ui->layerWindowDoubleSpinBox->blockSignals(false);

		_ui->layerWindowHorizontalSlider->blockSignals(true);
		_ui->layerWindowHorizontalSlider->setValue(singleRowSelection ? 100.0f * window : 100.0f);
		_ui->layerWindowHorizontalSlider->blockSignals(false);
	}

	const auto levelFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::LevelNormalized));

	_ui->layerLevelLabel->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelDoubleSpinBox->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelHorizontalSlider->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);

	if (columns.contains(to_underlying(Layer::Column::LevelNormalized))) {
		const auto level = _layersModel->data(topLeft.row(), to_underlying(Layer::Column::LevelNormalized), Qt::EditRole).toFloat();

		_ui->layerLevelDoubleSpinBox->blockSignals(true);
		_ui->layerLevelDoubleSpinBox->setValue(singleRowSelection ? level : 0.5f);
		_ui->layerLevelDoubleSpinBox->blockSignals(false);

		_ui->layerLevelHorizontalSlider->blockSignals(true);
		_ui->layerLevelHorizontalSlider->setValue(singleRowSelection ? 100.0f * level : 50.0f);
		_ui->layerLevelHorizontalSlider->blockSignals(false);
	}

	const auto maskFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::Mask));

	_ui->maskCheckBox->setEnabled(mightEdit && maskFlags & Qt::ItemIsEditable);

	const auto colorFlags = _layersModel->flags(topLeft.row(), to_underlying(Layer::Column::ColorMap));
	*/

	const auto type = _layersModel->data(topLeft.row(), to_underlying(LayerItem::Column::Type), Qt::EditRole).toInt();

	_ui->settingsStackedWidget->setCurrentIndex(type);

	/*
	_ui->pointsLayerWidget->updateData(topLeft, bottomRight, roles);
	_ui->clustersLayerWidget->updateData(topLeft, bottomRight, roles);
	_ui->selectionLayerWidget->updateData(topLeft, bottomRight, roles);
	*/
}