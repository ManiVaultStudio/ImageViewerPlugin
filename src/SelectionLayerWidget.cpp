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

	QObject::connect(_ui->colorPickerPushButton, &ColorPickerPushButton::currentColorChanged, [this](const QColor& currentColor) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::OverlayColor)), currentColor);
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
		
		_ui->groupBox->setEnabled(enabled);
		
		const auto overlayColorFlags = _layersModel->flags(topLeft.row(), ult(SelectionLayer::Column::OverlayColor));

		//_ui->colorLabel->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);
		//_ui->colorPickerPushButton->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);

		if (column == ult(SelectionLayer::Column::OverlayColor)) {
			const auto overlayColor = _layersModel->data(topLeft.row(), ult(SelectionLayer::Column::OverlayColor), Qt::EditRole).value<QColor>();

			_ui->colorPickerPushButton->blockSignals(true);
			_ui->colorPickerPushButton->setCurrentColor(overlayColor);
			_ui->colorPickerPushButton->blockSignals(false);
		}
	}
}