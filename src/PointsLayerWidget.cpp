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

	QObject::connect(_ui->channel2CheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled)), state == Qt::Checked);
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)), _ui->channel2ComboBox->currentIndex());
	});

	QObject::connect(_ui->channel3CheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled)), state == Qt::Checked);
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)), _ui->channel3ComboBox->currentIndex());
	});

	QObject::connect(_ui->alphaCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel4Enabled)), state == Qt::Checked);
	});

	QObject::connect(_ui->alphaInvertCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel4Inverted)), state == Qt::Checked);
	});

	QObject::connect(_ui->channel1ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel1DimensionId)), index);
	});

	QObject::connect(_ui->channel2ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)), index);
	});

	QObject::connect(_ui->channel3ComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)), index);
	});

	QObject::connect(_ui->alphaComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel4DimensionId)), index);
	});

	QObject::connect(_ui->solidColorCheckBox, &QCheckBox::stateChanged, [this](int state) {
		switch (state)
		{
			case Qt::Unchecked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::SolidColor)), false);
				break;

			case Qt::Checked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::SolidColor)), true);
				break;
		}
	});

	_ui->colormapComboBox->setModel(&_imageViewerPlugin->colorMapModel());
	_ui->colormapComboBox->setType(ColorMap::Type::TwoDimensional);

	QObject::connect(_ui->colormapComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ColorMap)), _ui->colormapComboBox->currentImage());
	});
}

void PointsLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows = _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows = selectedRows.size();

	if (noSelectedRows != 1)
		return;

	if (selectedRows.first().row() != topLeft.row())
		return;

	const auto enabled = _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index = topLeft.siblingAtColumn(column);

		auto validSelection = false;
		auto flags = 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection = true;
			flags = _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Flags)), Qt::EditRole).toInt();
		}

		const auto mightEdit = validSelection && enabled;

		_ui->groupBox->setEnabled(enabled);

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

			_ui->alphaComboBox->blockSignals(true);
			_ui->alphaComboBox->setModel(dimensionNamesModel);
			_ui->alphaComboBox->setCurrentIndex(std::min(2, dimensionNames.count()));
			_ui->alphaComboBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::Channel1DimensionId)) {
			const auto channel1Flags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1DimensionId)));

			_ui->channel1Label->setEnabled(channel1Flags & Qt::ItemIsEditable);
			_ui->channel1ComboBox->setEnabled(channel1Flags & Qt::ItemIsEditable);
		}

		if (column == ult(PointsLayer::Column::Channel2DimensionId)) {
			const auto channel2Flags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)));

			_ui->channel2Label->setEnabled(channel2Flags & Qt::ItemIsEditable);
			_ui->channel2ComboBox->setEnabled(channel2Flags & Qt::ItemIsEditable);
		}

		if (column == ult(PointsLayer::Column::Channel3DimensionId)) {
			const auto channel3Flags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)));

			_ui->channel3Label->setEnabled(channel3Flags & Qt::ItemIsEditable);
			_ui->channel3ComboBox->setEnabled(channel3Flags & Qt::ItemIsEditable);
		}

		if (column == ult(PointsLayer::Column::Channel3DimensionId)) {
			const auto channel4Flags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel4DimensionId)));

			_ui->alphaLabel->setEnabled(channel4Flags & Qt::ItemIsEditable);
			_ui->alphaComboBox->setEnabled(channel4Flags & Qt::ItemIsEditable);
			_ui->alphaInvertCheckBox->setEnabled(channel4Flags & Qt::ItemIsEditable);
		}

		if (column == ult(PointsLayer::Column::Channel1Enabled)) {
			const auto channel1Enabled		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1Enabled)), Qt::EditRole).toBool();
			const auto channel1EnabledFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1Enabled)));

			_ui->channel1CheckBox->setEnabled(channel1EnabledFlags & Qt::ItemIsEditable);
			_ui->channel1CheckBox->blockSignals(true);
			_ui->channel1CheckBox->setChecked(channel1Enabled);
			_ui->channel1CheckBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::Channel2Enabled)) {
			const auto channel2Enabled		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled)), Qt::EditRole).toBool();
			const auto channel2EnabledFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled)));

			_ui->channel2CheckBox->setEnabled(channel2EnabledFlags & Qt::ItemIsEditable);
			_ui->channel2CheckBox->blockSignals(true);
			_ui->channel2CheckBox->setChecked(channel2Enabled);
			_ui->channel2CheckBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::Channel3Enabled)) {
			const auto channel3Enabled		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled)), Qt::EditRole).toBool();
			const auto channel3EnabledFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled)));

			_ui->channel3CheckBox->setEnabled(channel3EnabledFlags & Qt::ItemIsEditable);
			_ui->channel3CheckBox->blockSignals(true);
			_ui->channel3CheckBox->setChecked(channel3Enabled);
			_ui->channel3CheckBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::Channel4Enabled)) {
			const auto channel4Enabled = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel4Enabled)), Qt::EditRole).toBool();
			const auto channel4EnabledFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel4Enabled)));

			_ui->alphaCheckBox->setEnabled(channel4EnabledFlags & Qt::ItemIsEditable);
			_ui->alphaCheckBox->blockSignals(true);
			_ui->alphaCheckBox->setChecked(channel4Enabled);
			_ui->alphaCheckBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::NoChannels)) {
			const auto maxNoChannels	= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::MaxNoChannels)), Qt::EditRole).toInt();
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

		if (column == ult(LayerNode::Column::SelectionSize)) {
			_ui->selectionSizeLineEdit->blockSignals(true);
			_ui->selectionSizeLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::SelectionSize)), Qt::EditRole).toInt()));
			_ui->selectionSizeLineEdit->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::ColorMap)) {
			const auto noChannels		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoChannels)), Qt::EditRole).toInt();
			const auto colorMapFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::ColorMap)));

			_ui->colormapLabel->setEnabled(colorMapFlags & Qt::ItemIsEditable);
			_ui->colormapComboBox->setEnabled(colorMapFlags & Qt::ItemIsEditable);

			_ui->colormapLabel->setText("Colormap");

			if (noChannels == 1)
				_ui->colormapLabel->setText("1D Colormap");

			if (noChannels == 2)
				_ui->colormapLabel->setText("2D Colormap");

			_ui->colormapComboBox->setType(static_cast<ColorMap::Type>(noChannels));
		}
	}
}