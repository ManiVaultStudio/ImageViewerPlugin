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
	_layersModel = &_imageViewerPlugin->getLayersModel();

	_ui->pointsLayerWidget->initialize(_imageViewerPlugin);
	_ui->selectionLayerWidget->initialize(_imageViewerPlugin);

	_ui->settingsStackedWidget->setVisible(false);

	QObject::connect(_ui->visibleCheckBox, &QCheckBox::stateChanged, [this](int state) {
		for (auto selectedRow : _layersModel->getSelectionModel().selectedRows()) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Name)), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->nameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Name)), text);
		}
	});

	QObject::connect(_ui->opacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto selectedRow : _layersModel->getSelectionModel().selectedRows()) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Opacity)), value / 100.0f);
		}
	});

	QObject::connect(_ui->opacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto selectedRow : _layersModel->getSelectionModel().selectedRows()) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Opacity)), static_cast<float>(value) / 100.0f);
		}
	});

	QObject::connect(_ui->scaleDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto selectedRow : _layersModel->getSelectionModel().selectedRows()) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Scale)), static_cast<float>(value) / 100.0f);
		}
	});

	QObject::connect(_ui->scaleHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto selectedRow : _layersModel->getSelectionModel().selectedRows()) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Scale)), static_cast<float>(value) / 100.0f);
		}
	});

	QObject::connect(_ui->resetPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		for (auto selectedRow : selectedRows) {
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Opacity)), 1.0f);
			_layersModel->setData(selectedRow.siblingAtColumn(ult(Layer::Column::Scale)), 1.0f);
		}
	});

	QObject::connect(_ui->zoomExtentsPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->getLayer(selectedRows.first())->zoomExtents();
		}
	});

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &LayerWidget::updateData);

	QObject::connect(&_layersModel->getSelectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.isEmpty()) {
			updateData(QModelIndex(), QModelIndex());
		}
		else {
			updateData(selectedRows.first().siblingAtColumn(ult(Layer::Column::Start)) , selectedRows.last().siblingAtColumn(ult(Layer::Column::End)));
		}
	});
}

void LayerWidget::updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->getSelectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto showLayerEditor	= noSelectedRows > 0;

	_ui->commonGroupBox->setVisible(showLayerEditor);
	_ui->commonGroupBox->setEnabled(showLayerEditor);
	_ui->navigationGroupBox->setVisible(false);

	_ui->settingsStackedWidget->setVisible(selectedRows.count() == 1);

	if (!selectedRows.isEmpty())
		_ui->settingsStackedWidget->setCurrentIndex(selectedRows.first().siblingAtColumn(ult(Layer::Column::Type)).data(Qt::EditRole).toInt());

	for (int column = begin.column(); column <= end.column(); column++) {
		if (column == ult(Layer::Column::Name)) {
			auto visible = QVector<int>();

			for (auto selectedRow : selectedRows)
				visible << (selectedRow.siblingAtColumn(ult(Layer::Column::Name)).data(Qt::CheckStateRole).toInt() == Qt::Checked ? 1 : 0);

			const auto noVisible = std::accumulate(visible.begin(), visible.end(), 0);

			_ui->visibleCheckBox->blockSignals(true);

			if (noVisible == 0)
				_ui->visibleCheckBox->setCheckState(Qt::Unchecked);
			else if (noVisible == visible.count())
				_ui->visibleCheckBox->setCheckState(Qt::Checked);
			else
				_ui->visibleCheckBox->setCheckState(Qt::PartiallyChecked);

			_ui->visibleCheckBox->blockSignals(false);

			const auto name = noSelectedRows == 1 ? _layersModel->data(end.siblingAtColumn(ult(Layer::Column::Name)), Qt::EditRole).toString() : "...";

			_ui->nameLabel->setEnabled(noSelectedRows == 1);
			_ui->nameLineEdit->setEnabled(noSelectedRows == 1);

			if (name != _ui->nameLineEdit->text()) {
				_ui->nameLineEdit->blockSignals(true);
				_ui->nameLineEdit->setText(name);
				_ui->nameLineEdit->blockSignals(false);
			}
		}

		const auto opacity = end.siblingAtColumn(ult(Layer::Column::Opacity)).data(Qt::EditRole).toFloat();

		if (column == ult(Layer::Column::Opacity)) {
			_ui->opacityDoubleSpinBox->blockSignals(true);
			_ui->opacityDoubleSpinBox->setValue(100.0f * opacity);
			_ui->opacityDoubleSpinBox->blockSignals(false);

			_ui->opacityHorizontalSlider->blockSignals(true);
			_ui->opacityHorizontalSlider->setValue(100.0f * opacity);
			_ui->opacityHorizontalSlider->blockSignals(false);
		}
		
		const auto scale = end.siblingAtColumn(ult(Layer::Column::Scale)).data(Qt::EditRole).toFloat();

		if (column == ult(Layer::Column::Scale)) {
			

			_ui->scaleDoubleSpinBox->blockSignals(true);
			_ui->scaleDoubleSpinBox->setValue(100.0f * scale);
			_ui->scaleDoubleSpinBox->blockSignals(false);

			_ui->scaleHorizontalSlider->blockSignals(true);
			_ui->scaleHorizontalSlider->setValue(100.0f * scale);
			_ui->scaleHorizontalSlider->blockSignals(false);
		}
		
		if (column == ult(Layer::Column::Opacity) || column == ult(Layer::Column::Scale)) {
			_ui->resetPushButton->setEnabled(opacity != 1.0f || scale != 1.0f);
		}
	}
}