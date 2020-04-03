#include "PointsLayerWidget.h"
#include "LayersModel.h"
#include "PointsLayer.h"

#include "ui_PointsLayerWidget.h"

#include <QDebug>

PointsLayerWidget::PointsLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::PointsLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void PointsLayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &PointsLayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else {
			const auto first = selected.indexes().first();
			updateData(first.siblingAtColumn(ult(PointsLayer::Column::Start)), first.siblingAtColumn(ult(PointsLayer::Column::End)));
		}
	});

	QObject::connect(_ui->widthSpinBox, qOverload<int>(&QSpinBox::valueChanged), [this](int value) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Width)), value);
	});

	QObject::connect(_ui->heightSpinBox, qOverload<int>(&QSpinBox::valueChanged), [this](int value) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Height)), value);
	});

	QObject::connect(_ui->squareCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Square)), state);
	});

	QObject::connect(_ui->channel2CheckBox, &QCheckBox::stateChanged, [this](int state) {
		switch (state)
		{
			case Qt::Unchecked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::NoChannels)), 1);
				break;

			case Qt::Checked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::NoChannels)), 2);
				break;
		}
		
	});

	QObject::connect(_ui->channel3CheckBox, &QCheckBox::stateChanged, [this](int state) {
		switch (state)
		{
			case Qt::Unchecked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::NoChannels)), 2);
				break;

			case Qt::Checked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::NoChannels)), 3);
				break;
		}

	});
}

void PointsLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows = _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows = selectedRows.size();
	const auto enabled = _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index = topLeft.siblingAtColumn(column);

		auto validSelection = false;
		auto flags = 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection = true;
			flags = _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Flags)), Qt::EditRole).toInt();
		}

		const auto mightEdit = validSelection && enabled;

		_ui->groupBox->setEnabled(enabled);
		_ui->heightSpinBox->setEnabled(enabled);

		const auto widthFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Width)));

		_ui->widthSpinBox->setEnabled(mightEdit && widthFlags & Qt::ItemIsEditable);

		if (column == ult(PointsLayer::Column::Width)) {
			const auto width = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Width)), Qt::EditRole).toInt();

			_ui->widthSpinBox->blockSignals(true);
			_ui->widthSpinBox->setValue(width);
			_ui->widthSpinBox->blockSignals(false);
		}

		const auto heightFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Height)));

		_ui->heightSpinBox->setEnabled(mightEdit && heightFlags & Qt::ItemIsEditable);

		if (column == ult(PointsLayer::Column::Height)) {
			const auto height = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Height)), Qt::EditRole).toInt();

			_ui->heightSpinBox->blockSignals(true);
			_ui->heightSpinBox->setValue(height);
			_ui->heightSpinBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::Square)) {
			const auto square = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Square)), Qt::EditRole).toBool();

			_ui->squareCheckBox->blockSignals(true);
			_ui->squareCheckBox->setChecked(square);
			_ui->squareCheckBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::NoChannels)) {
			const auto noChannels = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoChannels)), Qt::EditRole).toInt();

			_ui->channel2Label->setEnabled(noChannels >= 2);
			_ui->channel2CheckBox->setEnabled(noChannels >= 1);
			_ui->channel2CheckBox->blockSignals(true);
			_ui->channel2CheckBox->setChecked(noChannels >= 2);
			_ui->channel2CheckBox->blockSignals(false);
			_ui->channel2ComboBox->setEnabled(noChannels >= 2);

			_ui->channel3Label->setEnabled(noChannels == 3);
			_ui->channel3CheckBox->setEnabled(noChannels >= 2);
			_ui->channel3CheckBox->blockSignals(true);
			_ui->channel3CheckBox->setChecked(noChannels == 3);
			_ui->channel3CheckBox->blockSignals(false);
			_ui->channel3ComboBox->setEnabled(noChannels == 3);
			_ui->colormapLabel->setEnabled(noChannels <= 2);
			_ui->colormapComboBox->setEnabled(noChannels <= 2);

			_ui->colormapLabel->setText("Colormap");

			if (noChannels == 1)
				_ui->colormapLabel->setText("1D Colormap");

			if (noChannels == 2)
				_ui->colormapLabel->setText("2D Colormap");
		}

		if (column == ult(PointsLayer::Column::NoPoints)) {
			_ui->noPointsLineEdit->blockSignals(true);
			_ui->noPointsLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoPoints)), Qt::EditRole).toInt()));
			_ui->noPointsLineEdit->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::NoDimensions)) {
			_ui->noDimensionsLineEdit->blockSignals(true);
			_ui->noDimensionsLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoDimensions)), Qt::EditRole).toInt()));
			_ui->noDimensionsLineEdit->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::SelectionSize)) {
			_ui->selectionSizeLineEdit->blockSignals(true);
			_ui->selectionSizeLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::SelectionSize)), Qt::EditRole).toInt()));
			_ui->selectionSizeLineEdit->blockSignals(false);
		}
	}
}