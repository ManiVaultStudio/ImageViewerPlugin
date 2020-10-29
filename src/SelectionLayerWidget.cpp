#include "SelectionLayerWidget.h"
#include "LayersModel.h"
#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"

#include "Application.h"

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
	_layersModel = &_imageViewerPlugin->getLayersModel();

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &SelectionLayerWidget::updateData);

	QObject::connect(&_layersModel->getSelectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else {
			const auto first = selectedRows.first();

			if (selectedRows.count() == 1 && _layersModel->data(first.siblingAtColumn(ult(Layer::Column::Type)), Qt::EditRole) == ult(Layer::Type::Selection)) {
				updateData(first.siblingAtColumn(ult(SelectionLayer::Column::Start)), first.siblingAtColumn(ult(SelectionLayer::Column::End)));
			}
		}
	});

	QObject::connect(_ui->pixelSelectionTypeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int index) {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::PixelSelectionType)), index);
	});

	QObject::connect(_ui->pixelSelectionModifierAddPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::PixelSelectionModifier)), ult(SelectionModifier::Add));
	});

	QObject::connect(_ui->pixelSelectionModifierRemovePushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::PixelSelectionModifier)), ult(SelectionModifier::Remove));
	});

	const auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

	_ui->pixelSelectionModifierAddPushButton->setFont(fontAwesome.getFont(9));
	_ui->pixelSelectionModifierRemovePushButton->setFont(fontAwesome.getFont(9));

	_ui->pixelSelectionModifierAddPushButton->setText(fontAwesome.getIconCharacter("plus-circle"));
	_ui->pixelSelectionModifierRemovePushButton->setText(fontAwesome.getIconCharacter("minus-circle"));

	_ui->brushRadiusDoubleSpinBox->setMinimum(static_cast<double>(SelectionLayer::minBrushRadius));
	_ui->brushRadiusDoubleSpinBox->setMaximum(static_cast<double>(SelectionLayer::maxBrushRadius));

	_ui->brushRadiusHorizontalSlider->setMinimum(static_cast<int>(SelectionLayer::minBrushRadius));
	_ui->brushRadiusHorizontalSlider->setMaximum(static_cast<int>(SelectionLayer::maxBrushRadius));

	QObject::connect(_ui->brushRadiusDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.count() == 1)
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(SelectionLayer::Column::BrushRadius)), value);
	});

	QObject::connect(_ui->brushRadiusHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->getSelectionModel().selectedRows();

		if (selectedRows.count() == 1)
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(SelectionLayer::Column::BrushRadius)), value);
	});

	QObject::connect(_ui->selectAllPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::SelectAll)), QVariant());
	});

	QObject::connect(_ui->selectNonePushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::SelectNone)), QVariant());
	});

	QObject::connect(_ui->invertSelectionPushButton, &QPushButton::clicked, [this]() {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::InvertSelection)), QVariant());
	});

	QObject::connect(_ui->colorPickerPushButton, &ColorPickerPushButton::colorChanged, [this](const QColor& currentColor) {
		_layersModel->setData(_layersModel->getSelectionModel().currentIndex().siblingAtColumn(ult(SelectionLayer::Column::OverlayColor)), currentColor);
	});

	QObject::connect(_ui->subsetFromSelectedPixelsPushButton, &QPushButton::clicked, [this](const QColor& currentColor) {
		static_cast<SelectionLayer*>(_layersModel->getSelectedLayer())->subsetFromSelectedPixels();
	});

	_ui->subsetFromSelectionBoundsPushButton->hide();

	QObject::connect(_ui->subsetFromSelectionBoundsPushButton, &QPushButton::clicked, [this](const QColor& currentColor) {
		static_cast<SelectionLayer*>(_layersModel->getSelectedLayer())->subsetFromSelectionBounds();
	});
}

void SelectionLayerWidget::updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->getSelectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto enabled			= _layersModel->data(begin.siblingAtColumn(ult(Layer::Column::Name)), Qt::CheckStateRole).toBool();

	for (int column = begin.column(); column <= end.column(); column++) {
		const auto index = _layersModel->index(begin.row(), column);

		auto validSelection = false;
		auto flags = 0;

		if (index.isValid() && noSelectedRows == 1) {
			validSelection = true;
			flags = _layersModel->data(begin.row(), ult(Layer::Column::Flags), Qt::EditRole).toInt();
		}

		const auto mightEdit = validSelection && enabled;
		
		_ui->pixelSelectionGroupBox->setEnabled(enabled);
		_ui->displayGroupBox->setEnabled(enabled);
		
		const auto overlayColorFlags = _layersModel->flags(begin.row(), ult(SelectionLayer::Column::OverlayColor));

		//_ui->colorLabel->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);
		//_ui->colorPickerPushButton->setEnabled(mightEdit && overlayColorFlags & Qt::ItemIsEditable);

		if (column == ult(SelectionLayer::Column::PixelSelectionType)) {
			const auto pixelSelectionType		= _layersModel->data(begin.row(), ult(SelectionLayer::Column::PixelSelectionType), Qt::EditRole).toInt();
			const auto pixelSelectionTypeFlags	= _layersModel->flags(begin.row(), ult(SelectionLayer::Column::PixelSelectionType));

			_ui->pixelSelectionTypeComboBox->setEnabled(pixelSelectionTypeFlags & Qt::ItemIsEditable);

			_ui->pixelSelectionTypeComboBox->blockSignals(true);
			_ui->pixelSelectionTypeComboBox->setCurrentIndex(pixelSelectionType);
			_ui->pixelSelectionTypeComboBox->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::PixelSelectionModifier)) {
			const auto pixelSelectionModifier		= _layersModel->data(begin.row(), ult(SelectionLayer::Column::PixelSelectionModifier), Qt::EditRole).toInt();
			const auto pixelSelectionModifierFlags	= _layersModel->flags(begin.row(), ult(SelectionLayer::Column::PixelSelectionModifier));

			_ui->pixelSelectionModifierAddPushButton->blockSignals(true);
			_ui->pixelSelectionModifierAddPushButton->setChecked(pixelSelectionModifier == ult(SelectionModifier::Add));
			_ui->pixelSelectionModifierAddPushButton->blockSignals(false);

			_ui->pixelSelectionModifierRemovePushButton->blockSignals(true);
			_ui->pixelSelectionModifierRemovePushButton->setChecked(pixelSelectionModifier == ult(SelectionModifier::Remove));
			_ui->pixelSelectionModifierRemovePushButton->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::BrushRadius)) {
			const auto brushRadius		= _layersModel->data(begin.row(), ult(SelectionLayer::Column::BrushRadius), Qt::EditRole).toFloat();
			const auto brushRadiusFlags	= _layersModel->flags(begin.row(), ult(SelectionLayer::Column::BrushRadius));

			_ui->brushRadiusLabel->setEnabled(brushRadiusFlags & Qt::ItemIsEditable);
			_ui->brushRadiusDoubleSpinBox->setEnabled(brushRadiusFlags & Qt::ItemIsEditable);
			_ui->brushRadiusHorizontalSlider->setEnabled(brushRadiusFlags & Qt::ItemIsEditable);

			_ui->brushRadiusDoubleSpinBox->blockSignals(true);
			_ui->brushRadiusDoubleSpinBox->setValue(brushRadius);
			_ui->brushRadiusDoubleSpinBox->blockSignals(false);

			_ui->brushRadiusHorizontalSlider->blockSignals(true);
			_ui->brushRadiusHorizontalSlider->setValue(brushRadius);
			_ui->brushRadiusHorizontalSlider->blockSignals(false);
		}

		if (column == ult(SelectionLayer::Column::SelectAll)) {
			const auto selectAllFlags = _layersModel->flags(begin.row(), ult(SelectionLayer::Column::SelectAll));

			_ui->selectAllPushButton->setEnabled(selectAllFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::SelectNone)) {
			const auto selectNoneFlags = _layersModel->flags(begin.row(), ult(SelectionLayer::Column::SelectNone));

			_ui->selectNonePushButton->setEnabled(selectNoneFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::InvertSelection)) {
			const auto invertSelectionFlags = _layersModel->flags(begin.row(), ult(SelectionLayer::Column::InvertSelection));

			_ui->invertSelectionPushButton->setEnabled(invertSelectionFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::CreateSubset)) {
			const auto createSubsetFlags = _layersModel->flags(begin.row(), ult(SelectionLayer::Column::CreateSubset));

			_ui->subsetFromSelectedPixelsPushButton->setEnabled(createSubsetFlags & Qt::ItemIsEditable);
			_ui->subsetFromSelectionBoundsPushButton->setEnabled(createSubsetFlags & Qt::ItemIsEditable);
		}

		if (column == ult(SelectionLayer::Column::OverlayColor)) {
			const auto overlayColor = _layersModel->data(begin.row(), ult(SelectionLayer::Column::OverlayColor), Qt::EditRole).value<QColor>();

			_ui->colorPickerPushButton->blockSignals(true);
			_ui->colorPickerPushButton->setColor(overlayColor);
			_ui->colorPickerPushButton->blockSignals(false);
		}
	}
}