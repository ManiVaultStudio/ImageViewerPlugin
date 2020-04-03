#include "LayerWidget.h"
#include "LayersModel.h"
#include "Layer.h"

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

void LayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	_ui->maskCheckBox->setVisible(false);

	_ui->pointsLayerWidget->initialize(_layersModel);
	_ui->imagesLayerWidget->initialize(_layersModel);
	_ui->clustersLayerWidget->initialize(_layersModel);
	_ui->selectionLayerWidget->initialize(_layersModel);

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int state) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();
		
		if (selectedRows.count() == 1) {
			_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(Layer::Column::Name)), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->layerNameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Name)), text);
		}
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), static_cast<float>(value) / static_cast<float>(range));
		}
	});

	
	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::WindowNormalized)), value);
		}
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::WindowNormalized)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::LevelNormalized)), value);
		}
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::LevelNormalized)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->resetWindowLevelPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Opacity)), 1.0f);
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::WindowNormalized)), 1.0f);
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::LevelNormalized)), 0.5f);
		}
	});

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &LayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else
			updateData(selected.indexes().first(), selected.indexes().last());
	});
}

void LayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto enabled			= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

	_ui->generalGroupBox->setVisible(noSelectedRows == 1);
	_ui->generalGroupBox->setEnabled(noSelectedRows == 1);

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
			_ui->layerEnabledCheckBox->setEnabled(noSelectedRows == 1);
			_ui->layerEnabledCheckBox->blockSignals(true);
			_ui->layerEnabledCheckBox->setChecked(enabled);
			_ui->layerEnabledCheckBox->blockSignals(false);

			const auto nameFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::Name)));
			const auto name			= validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::EditRole).toString() : "";

			_ui->layerNameLabel->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);
			_ui->layerNameLineEdit->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);

			if (name != _ui->layerNameLineEdit->text()) {
				_ui->layerNameLineEdit->blockSignals(true);
				_ui->layerNameLineEdit->setText(name);
				_ui->layerNameLineEdit->blockSignals(false);
			}
		}

		const auto opacityFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)));

		_ui->layerOpacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

		if (column == ult(Layer::Column::Opacity)) {
			const auto opacity = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)), Qt::EditRole).toFloat() : 1.0f;

			_ui->layerOpacityDoubleSpinBox->blockSignals(true);
			_ui->layerOpacityDoubleSpinBox->setValue(100.0f * opacity);
			_ui->layerOpacityDoubleSpinBox->blockSignals(false);

			_ui->layerOpacityHorizontalSlider->blockSignals(true);
			_ui->layerOpacityHorizontalSlider->setValue(100.0f * opacity);
			_ui->layerOpacityHorizontalSlider->blockSignals(false);
		}

		if (column == ult(Layer::Column::WindowNormalized)) {
			const auto windowFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::WindowNormalized)));

			_ui->layerWindowLabel->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
			_ui->layerWindowDoubleSpinBox->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
			_ui->layerWindowHorizontalSlider->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);

			const auto window = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::WindowNormalized)), Qt::EditRole).toFloat() : 1.0f;

			_ui->layerWindowDoubleSpinBox->blockSignals(true);
			_ui->layerWindowDoubleSpinBox->setValue(window);
			_ui->layerWindowDoubleSpinBox->blockSignals(false);

			_ui->layerWindowHorizontalSlider->blockSignals(true);
			_ui->layerWindowHorizontalSlider->setValue(100.0f * window);
			_ui->layerWindowHorizontalSlider->blockSignals(false);
		}

		if (column == ult(Layer::Column::LevelNormalized)) {
			const auto levelFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(Layer::Column::LevelNormalized)));

			_ui->layerLevelLabel->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
			_ui->layerLevelDoubleSpinBox->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
			_ui->layerLevelHorizontalSlider->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);

			const auto level = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::LevelNormalized)), Qt::EditRole).toFloat() : 0.5f;

			_ui->layerLevelDoubleSpinBox->blockSignals(true);
			_ui->layerLevelDoubleSpinBox->setValue(level);
			_ui->layerLevelDoubleSpinBox->blockSignals(false);

			_ui->layerLevelHorizontalSlider->blockSignals(true);
			_ui->layerLevelHorizontalSlider->setValue(100.0f * level);
			_ui->layerLevelHorizontalSlider->blockSignals(false);
		}

		if (column == ult(Layer::Column::Opacity) || column == ult(Layer::Column::WindowNormalized) || column == ult(Layer::Column::LevelNormalized)) {
			const auto opacity	= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Opacity)), Qt::EditRole).toFloat();
			const auto window	= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::WindowNormalized)), Qt::EditRole).toFloat();
			const auto level	= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::LevelNormalized)), Qt::EditRole).toFloat();

			_ui->resetWindowLevelPushButton->setEnabled(opacity != 1.0f || window != 1.0f || level != 0.5f);
		}
	}

	const auto type = topLeft.siblingAtColumn(ult(Layer::Column::Type)).data(Qt::EditRole).toInt();

	_ui->settingsStackedWidget->setVisible(noSelectedRows == 1);
	_ui->settingsStackedWidget->setCurrentIndex(type);
}