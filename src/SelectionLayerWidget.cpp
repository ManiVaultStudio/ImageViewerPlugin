#include "SelectionLayerWidget.h"
#include "LayersModel.h"
#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"
#include "ColorPickerPushButton.h"

#include "ui_SelectionLayerWidget.h"

#include <QDebug>
#include <QImage>

SelectionLayerWidget::SelectionLayerWidget(QWidget* parent) :
	_ui{ std::make_unique<Ui::SelectionLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void SelectionLayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;
	_layersModel = &_imageViewerPlugin->layersModel();

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &SelectionLayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else {
			const auto first = selected.indexes().first();
			updateData(first.siblingAtColumn(ult(SelectionLayer::Column::Start)), first.siblingAtColumn(ult(SelectionLayer::Column::End)));
		}
	});

	QObject::connect(_ui->pixelSelectionModifierAddPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::PixelSelectionModifier)), ult(SelectionModifier::Add));
	});

	QObject::connect(_ui->pixelSelectionModifierRemovePushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::PixelSelectionModifier)), ult(SelectionModifier::Remove));
	});

	QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::SelectAll)), QVariant());
	});

	QObject::connect(_ui->selectNonePushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::SelectNone)), QVariant());
	});

	QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::InvertSelection)), QVariant());
	});

	QObject::connect(_ui->colorPickerPushButton, &ColorPickerPushButton::currentColorChanged, [this](const QColor& currentColor) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::OverlayColor)), currentColor);
	});

	QObject::connect(_ui->autoZoomToSelectionCheckBox, &QCheckBox::stateChanged, [this](int state) {
		switch (state)
		{
			case Qt::Unchecked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::AutoZoomToSelection)), false);
				break;

			case Qt::Checked:
				_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::AutoZoomToSelection)), true);
				break;
		}
	});
}

void SelectionLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto enabled			= _layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::Name)), Qt::CheckStateRole).toBool();

	for (int column = topLeft.column(); column <= bottomRight.column(); column++) {
		const auto index = _layersModel->index(topLeft.row(), column);

		auto validSelection = false;
		auto flags = 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection = true;
			flags = _layersModel->data(topLeft.row(), ult(LayerNode::Column::Flags), Qt::EditRole).toInt();
		}

		const auto mightEdit = validSelection && enabled;
		
		_ui->pixelSelectionGroupBox->setEnabled(enabled);
		_ui->displayGroupBox->setEnabled(enabled);
		
		const auto overlayColorFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::OverlayColor));

		//_ui->colorLabel->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);
		//_ui->colorPickerPushButton->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);

		if (column == ult(SelectionLayer::Column::PixelSelectionType)) {
			const auto pixelSelectionType		= _layersModel->data(topLeft.row(), ult(SelectionLayer::Column::PixelSelectionType), Qt::EditRole).toInt();
			const auto pixelSelectionTypeFlags	= _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::PixelSelectionType));

			_ui->pixelSelectionTypeComboBox->setEnabled(pixelSelectionTypeFlags & Qt::ItemIsEditable);

			_ui->pixelSelectionTypeComboBox->blockSignals(true);
			_ui->pixelSelectionTypeComboBox->setCurrentIndex(pixelSelectionType);
			_ui->pixelSelectionTypeComboBox->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::PixelSelectionModifier)) {
			const auto pixelSelectionModifier		= _layersModel->data(topLeft.row(), ult(SelectionLayer::Column::PixelSelectionModifier), Qt::EditRole).toInt();
			const auto pixelSelectionModifierFlags	= _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::PixelSelectionModifier));

			_ui->pixelSelectionModifierAddPushButton->blockSignals(true);
			_ui->pixelSelectionModifierAddPushButton->setChecked(pixelSelectionModifier == ult(SelectionModifier::Add));
			_ui->pixelSelectionModifierAddPushButton->blockSignals(false);

			_ui->pixelSelectionModifierRemovePushButton->blockSignals(true);
			_ui->pixelSelectionModifierRemovePushButton->setChecked(pixelSelectionModifier == ult(SelectionModifier::Remove));
			_ui->pixelSelectionModifierRemovePushButton->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::SelectAll)) {
			const auto selectAllFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::SelectAll));

			_ui->selectAllPushButton->setEnabled(selectAllFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::SelectNone)) {
			const auto selectNoneFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::SelectNone));

			_ui->selectNonePushButton->setEnabled(selectNoneFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::InvertSelection)) {
			const auto invertSelectionFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::InvertSelection));

			_ui->invertSelectionPushButton->setEnabled(invertSelectionFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::AutoZoomToSelection)) {
			const auto autoZoomToSelection		= _layersModel->data(topLeft.row(), ult(SelectionLayer::Column::AutoZoomToSelection), Qt::EditRole).toBool();
			const auto autoZoomToSelectionFlags	= _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::AutoZoomToSelection));

			_ui->autoZoomToSelectionCheckBox->setEnabled(autoZoomToSelectionFlags & Qt::ItemIsEditable);

			_ui->autoZoomToSelectionCheckBox->blockSignals(true);
			_ui->autoZoomToSelectionCheckBox->setChecked(autoZoomToSelection);
			_ui->autoZoomToSelectionCheckBox->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::ZoomToSelection)) {
			const auto zoomToSelectionFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::ZoomToSelection));

			_ui->zoomToSelectionPushButton->setEnabled(zoomToSelectionFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::OverlayColor)) {
			const auto overlayColor = _layersModel->data(topLeft.row(), ult(SelectionLayer::Column::OverlayColor), Qt::EditRole).value<QColor>();

			_ui->colorPickerPushButton->blockSignals(true);
			_ui->colorPickerPushButton->setCurrentColor(overlayColor);
			_ui->colorPickerPushButton->blockSignals(false);
		}
	}
}