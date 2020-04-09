#include "PointsLayerWidget.h"
#include "LayersModel.h"
#include "PointsLayer.h"
#include "ImageViewerPlugin.h"

#include "ui_PointsLayerWidget.h"

#include <QDebug>
#include <QStringListModel>

PointsLayerWidget::PointsLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::PointsLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void PointsLayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;
	_layersModel = &_imageViewerPlugin->layersModel();

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

	_ui->colormapComboBox->setModel(&_imageViewerPlugin->colorMapModel());
	_ui->colormapComboBox->setType(ColorMap::Type::TwoDimensional);
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

		if (column == ult(PointsLayer::Column::DimensionNames)) {
			const auto dimensionNames = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::DimensionNames)), Qt::EditRole).toStringList();

			auto dimensionNamesModel = new QStringListModel(dimensionNames, this);

			_ui->channel1ComboBox->blockSignals(true);
			_ui->channel1ComboBox->setModel(dimensionNamesModel);
			_ui->channel1ComboBox->setCurrentIndex(0);
			_ui->channel1ComboBox->blockSignals(false);

			_ui->channel2ComboBox->blockSignals(true);
			_ui->channel2ComboBox->setModel(dimensionNamesModel);
			_ui->channel2ComboBox->setCurrentIndex(std::min(1, dimensionNames.count()));
			_ui->channel2ComboBox->blockSignals(false);

			_ui->channel3ComboBox->blockSignals(true);
			_ui->channel3ComboBox->setModel(dimensionNamesModel);
			_ui->channel3ComboBox->setCurrentIndex(std::min(2, dimensionNames.count()));
			_ui->channel3ComboBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::NoChannels)) {
			const auto noChannels		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoChannels)), Qt::EditRole).toInt();
			const auto channel1Flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1)));
			const auto channel2Flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2)));
			const auto channel3Flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3)));

			_ui->channel1Label->setEnabled(channel1Flags & Qt::ItemIsEditable);
			_ui->channel1CheckBox->blockSignals(true);
			_ui->channel1CheckBox->setChecked(channel1Flags & Qt::ItemIsEditable);
			_ui->channel1CheckBox->blockSignals(false);
			_ui->channel1ComboBox->setEnabled(channel1Flags & Qt::ItemIsEditable);

			_ui->channel2Label->setEnabled(channel2Flags & Qt::ItemIsEditable);
			_ui->channel2CheckBox->setEnabled(noChannels >= 1);
			_ui->channel2CheckBox->blockSignals(true);
			_ui->channel2CheckBox->setChecked(channel2Flags & Qt::ItemIsEditable);
			_ui->channel2CheckBox->blockSignals(false);
			_ui->channel2ComboBox->setEnabled(channel2Flags & Qt::ItemIsEditable);

			_ui->channel3Label->setEnabled(channel3Flags & Qt::ItemIsEditable);
			_ui->channel3CheckBox->setEnabled(noChannels >= 2);
			_ui->channel3CheckBox->blockSignals(true);
			_ui->channel3CheckBox->setChecked(channel3Flags & Qt::ItemIsEditable);
			_ui->channel3CheckBox->blockSignals(false);
			_ui->channel3ComboBox->setEnabled(channel3Flags & Qt::ItemIsEditable);

			_ui->colormapLabel->setEnabled(noChannels <= 2);
			_ui->colormapComboBox->setEnabled(noChannels <= 2);

			_ui->colormapLabel->setText("Colormap");

			if (noChannels == 1)
				_ui->colormapLabel->setText("1D Colormap");

			if (noChannels == 2)
				_ui->colormapLabel->setText("2D Colormap");

			_ui->colormapComboBox->setType(static_cast<ColorMap::Type>(noChannels));
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