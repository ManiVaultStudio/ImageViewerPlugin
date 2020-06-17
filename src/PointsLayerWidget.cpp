#include "PointsLayerWidget.h"
#include "LayersModel.h"
#include "PointsLayer.h"
#include "ImageViewerPlugin.h"
#include "WindowLevelWidget.h"

#include "ui_PointsLayerWidget.h"

#include <QDebug>
#include <QStringListModel>
#include <QWidgetAction>
#include <QListView>

PointsLayerWidget::PointsLayerWidget(QWidget* parent) :
	QWidget(parent),
	_ui{ std::make_unique<Ui::PointsLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void PointsLayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;
	_layersModel = &_imageViewerPlugin->layersModel();

	QFont font = QFont("Font Awesome 5 Free Solid", 9);

	_ui->channel1ComboBox->view()->setFixedWidth(300);
	_ui->channel2ComboBox->view()->setFixedWidth(300);
	_ui->channel3ComboBox->view()->setFixedWidth(300);

	_ui->channel1WindowLevelPushButton->setFont(font);
	_ui->channel2WindowLevelPushButton->setFont(font);
	_ui->channel3WindowLevelPushButton->setFont(font);

	_ui->channel1WindowLevelPushButton->setText(u8"\uf042");
	_ui->channel2WindowLevelPushButton->setText(u8"\uf042");
	_ui->channel3WindowLevelPushButton->setText(u8"\uf042");

	_ui->channel1ProbePushButton->setFont(font);
	_ui->channel2ProbePushButton->setFont(font);
	_ui->channel3ProbePushButton->setFont(font);

	_ui->channel1ProbePushButton->setText(u8"\uf1fb");
	_ui->channel2ProbePushButton->setText(u8"\uf1fb");
	_ui->channel3ProbePushButton->setText(u8"\uf1fb");

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

	const auto showWindowLevelWidget = [&, this](QWidget* parent, const QModelIndex& windowIndex, const QModelIndex& levelIndex) {
		auto windowLevelWidget = new WindowLevelWidget(parent, _imageViewerPlugin, windowIndex, levelIndex);

		windowLevelWidget->show();
	};

	QObject::connect(_ui->channel1WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
		const auto selectedRow	= _layersModel->selectionModel().selectedRows().first();
		const auto windowIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel1Window));
		const auto levelIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel1Level));

		showWindowLevelWidget(_ui->channel1WindowLevelPushButton, windowIndex, levelIndex);
	});

	QObject::connect(_ui->channel2WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
		const auto selectedRow	= _layersModel->selectionModel().selectedRows().first();
		const auto windowIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel2Window));
		const auto levelIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel2Level));

		showWindowLevelWidget(_ui->channel2WindowLevelPushButton, windowIndex, levelIndex);
	});

	QObject::connect(_ui->channel3WindowLevelPushButton, &QPushButton::clicked, [&, this, showWindowLevelWidget]() {
		const auto selectedRow	= _layersModel->selectionModel().selectedRows().first();
		const auto windowIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel3Window));
		const auto levelIndex	= selectedRow.siblingAtColumn(ult(PointsLayer::Column::Channel3Level));

		showWindowLevelWidget(_ui->channel3WindowLevelPushButton, windowIndex, levelIndex);
	});

	QObject::connect(_ui->channel2CheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2Enabled)), state == Qt::Checked);
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)), _ui->channel2ComboBox->currentIndex());
	});

	QObject::connect(_ui->channel3CheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3Enabled)), state == Qt::Checked);
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)), _ui->channel3ComboBox->currentIndex());
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

	QObject::connect(_ui->constantColorCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)), _ui->constantColorCheckBox->isChecked());
	});

	QObject::connect(_ui->colorSpaceComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ColorSpace)), index);
	});

	_ui->colorMapComboBox->setModel(&_imageViewerPlugin->colorMapModel());
	_ui->colorMapComboBox->setType(ColorMap::Type::TwoDimensional);

	QObject::connect(_ui->colorMapComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ColorMap)), _ui->colorMapComboBox->currentImage());
	});

	QObject::connect(_ui->colorPickerPushButton, &ColorPickerPushButton::currentColorChanged, [this](const QColor& currentColor) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::ConstantColor)), currentColor);
	});

	QObject::connect(_ui->pointTypeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::PointType)), index);
	});

	QObject::connect(_ui->indexSelectionComboBox, &QComboBox::currentTextChanged, [this](QString text) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(PointsLayer::Column::IndexSelectionDatasetName)), text);
	});

	QObject::connect(_imageViewerPlugin, &ImageViewerPlugin::pointsDatasetsChanged, [this](QStringList pointsDatasets) {

		//pointsDatasets.removeAll(_datasetName);

		_ui->indexSelectionComboBox->clear();
		_ui->indexSelectionComboBox->insertItems(0, pointsDatasets);
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

		if (column == ult(PointsLayer::Column::Channel1Name)) {
			const auto channel1Name			= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1Name)), Qt::DisplayRole).toString();
			const auto channel1NameFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1Name)));

			_ui->channel1Label->setEnabled(channel1NameFlags & Qt::ItemIsEditable);
			_ui->channel1Label->setText(channel1Name);
		}

		if (column == ult(PointsLayer::Column::Channel2Name)) {
			const auto channel2Name = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2Name)), Qt::DisplayRole).toString();
			const auto channel2NameFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2Name)));

			_ui->channel2Label->setEnabled(channel2NameFlags & Qt::ItemIsEditable);
			_ui->channel2Label->setText(channel2Name);
		}

		if (column == ult(PointsLayer::Column::Channel3Name)) {
			const auto channel3Name = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3Name)), Qt::DisplayRole).toString();
			const auto channel3NameFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3Name)));

			_ui->channel3Label->setEnabled(channel3NameFlags & Qt::ItemIsEditable);
			_ui->channel3Label->setText(channel3Name);
		}

		if (column == ult(PointsLayer::Column::Channel1DimensionId)) {
			const auto flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1DimensionId)));
			const auto enabled	= flags & Qt::ItemIsEditable;

			_ui->channel1Label->setEnabled(enabled);
			_ui->channel1ComboBox->setEnabled(enabled);
			_ui->channel1WindowLevelPushButton->setEnabled(enabled);
			_ui->channel1ProbePushButton->setEnabled(enabled);
		}

		if (column == ult(PointsLayer::Column::Channel2DimensionId)) {
			const auto flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel2DimensionId)));
			const auto enabled	= flags & Qt::ItemIsEditable;

			_ui->channel2Label->setEnabled(enabled);
			_ui->channel2ComboBox->setEnabled(enabled);
			_ui->channel2WindowLevelPushButton->setEnabled(enabled);
			_ui->channel2ProbePushButton->setEnabled(enabled);
		}

		if (column == ult(PointsLayer::Column::Channel3DimensionId)) {
			const auto flags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel3DimensionId)));
			const auto enabled	= flags & Qt::ItemIsEditable;

			_ui->channel3Label->setEnabled(enabled);
			_ui->channel3ComboBox->setEnabled(enabled);
			_ui->channel3WindowLevelPushButton->setEnabled(enabled);
			_ui->channel3ProbePushButton->setEnabled(enabled);
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

		if (column == ult(PointsLayer::Column::ColorSpace)) {
			const auto colorSpace		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::ColorSpace)), Qt::DisplayRole).toString();
			const auto colorSpaceFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::ColorSpace)));

			_ui->colorSpaceLabel->setEnabled(colorSpaceFlags & Qt::ItemIsEditable);
			_ui->colorSpaceComboBox->setEnabled(colorSpaceFlags & Qt::ItemIsEditable);

			_ui->colorSpaceComboBox->blockSignals(true);
			_ui->colorSpaceComboBox->setCurrentText(colorSpace);
			_ui->colorSpaceComboBox->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::ColorMap)) {
			const auto noChannels		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::NoChannels)), Qt::EditRole).toInt();
			const auto solidColor		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)), Qt::EditRole).toBool();
			const auto colorMapFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::ColorMap)));

			_ui->colormapLabel->setEnabled(colorMapFlags & Qt::ItemIsEditable);
			_ui->colorMapComboBox->setEnabled(colorMapFlags & Qt::ItemIsEditable);

			switch (noChannels)
			{
				case 1:
				{
					if (solidColor) {
						_ui->colormapLabel->setText("Constant color");
						//_ui->colorMapComboBox->setType(ColorMap::Type::ZeroDimensional);
					}
					else {
						_ui->colormapLabel->setText("1D color map");
						_ui->colorMapComboBox->setType(ColorMap::Type::OneDimensional);
					}

					break;
				}

				case 2:
				{
					_ui->colormapLabel->setText("2D Colormap");
					_ui->colorMapComboBox->setType(ColorMap::Type::TwoDimensional);
					break;
				}

				case 3:
				{
					_ui->colormapLabel->setText("Colormap");
					break;
				}

				default:
					break;
			}
		}

		if (column == ult(PointsLayer::Column::UseConstantColor)) {
			const auto useConstantColor	= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)), Qt::EditRole).toBool();
			const auto flags			= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::UseConstantColor)));

			_ui->constantColorCheckBox->setEnabled(flags & Qt::ItemIsEditable);
			_ui->constantColorCheckBox->blockSignals(true);
			_ui->constantColorCheckBox->setChecked(useConstantColor);
			_ui->constantColorCheckBox->blockSignals(false);

			_ui->colorStackedWidget->setCurrentIndex(useConstantColor ? 1 : 0);
		}

		if (column == ult(PointsLayer::Column::ConstantColor)) {
			const auto constantColor		= _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::ConstantColor)), Qt::EditRole).value<QColor>();
			const auto constantColorFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::ConstantColor)));

			_ui->colorPickerPushButton->blockSignals(true);
			_ui->colorPickerPushButton->setCurrentColor(constantColor);
			_ui->colorPickerPushButton->blockSignals(false);
		}

		if (column == ult(PointsLayer::Column::PointType)) {
		}

		if (column == ult(PointsLayer::Column::IndexSelectionDatasetName)) {
			//const auto channel1Name = _layersModel->data(topLeft.siblingAtColumn(ult(PointsLayer::Column::Channel1Name)), Qt::DisplayRole).toString();
			const auto flags = _layersModel->flags(topLeft.siblingAtColumn(ult(PointsLayer::Column::IndexSelectionDatasetName)));
			
			_ui->indexSelectionLabel->setEnabled(flags & Qt::ItemIsEditable);
			_ui->indexSelectionComboBox->setEnabled(flags & Qt::ItemIsEditable);
		}
	}
}