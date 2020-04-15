#include "ImagesLayerWidget.h"
#include "LayersModel.h"
#include "ImagesLayer.h"
#include "ImageViewerPlugin.h"

#include "ui_ImagesLayerWidget.h"

#include <QDebug>
#include <QStringListModel>

ImagesLayerWidget::ImagesLayerWidget(QWidget* parent) :
	QWidget(parent),
	_ui{ std::make_unique<Ui::ImagesLayerWidget>() },
	_layersModel(nullptr)
{
	_ui->setupUi(this);
}

void ImagesLayerWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;
	_layersModel = &_imageViewerPlugin->layersModel();

	QObject::connect(_ui->windowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)), value);
		}
	});

	QObject::connect(_ui->windowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->windowHorizontalSlider->maximum() - _ui->windowHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->levelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)), value);
		}
	});

	QObject::connect(_ui->levelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			const auto range = _ui->levelHorizontalSlider->maximum() - _ui->levelHorizontalSlider->minimum();
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->resetWindowLevelPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.count() == 1) {
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(LayerNode::Column::Opacity)), 1.0f);
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)), 1.0f);
			_layersModel->setData(selectedRows.first().siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)), 0.5f);
		}
	});

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImagesLayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else {
			const auto first = selected.indexes().first();
			updateData(first.siblingAtColumn(0), first.siblingAtColumn(ult(ImagesLayer::Column::End)));
		}
	});

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(ImagesLayer::Column::CurrentImageId)), currentIndex);
	});

	QObject::connect(_ui->averageCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().siblingAtColumn(ult(ImagesLayer::Column::Average)), state);
	});
}

void ImagesLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
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
		
		if (column == ult(ImagesLayer::Column::ImageSize)) {
			const auto imageSize = _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::ImageSize)), Qt::EditRole).toSize();

			_ui->imageSizeLineEdit->blockSignals(true);
			_ui->imageSizeLineEdit->setText(QString("[%1, %2]").arg(QString::number(imageSize.width()), QString::number(imageSize.height())));
			_ui->imageSizeLineEdit->blockSignals(false);
		}

		if (column == ult(ImagesLayer::Column::WindowNormalized)) {
			const auto windowFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)));

			_ui->windowLabel->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
			_ui->windowDoubleSpinBox->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
			_ui->windowHorizontalSlider->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);

			const auto window = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)), Qt::EditRole).toFloat() : 1.0f;

			_ui->windowDoubleSpinBox->blockSignals(true);
			_ui->windowDoubleSpinBox->setValue(window);
			_ui->windowDoubleSpinBox->blockSignals(false);

			_ui->windowHorizontalSlider->blockSignals(true);
			_ui->windowHorizontalSlider->setValue(100.0f * window);
			_ui->windowHorizontalSlider->blockSignals(false);
		}

		if (column == ult(ImagesLayer::Column::LevelNormalized)) {
			const auto levelFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)));

			_ui->levelLabel->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
			_ui->levelDoubleSpinBox->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
			_ui->levelHorizontalSlider->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);

			const auto level = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)), Qt::EditRole).toFloat() : 0.5f;

			_ui->levelDoubleSpinBox->blockSignals(true);
			_ui->levelDoubleSpinBox->setValue(level);
			_ui->levelDoubleSpinBox->blockSignals(false);

			_ui->levelHorizontalSlider->blockSignals(true);
			_ui->levelHorizontalSlider->setValue(100.0f * level);
			_ui->levelHorizontalSlider->blockSignals(false);
		}

		if (column == ult(ImagesLayer::Column::WindowNormalized) || column == ult(ImagesLayer::Column::LevelNormalized)) {
			const auto window	= _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::WindowNormalized)), Qt::EditRole).toFloat();
			const auto level	= _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::LevelNormalized)), Qt::EditRole).toFloat();

			_ui->resetWindowLevelPushButton->setEnabled(window != 1.0f || level != 0.5f);
		}

		if (column == ult(ImagesLayer::Column::NoPoints)) {
			_ui->noPointsLineEdit->blockSignals(true);
			_ui->noPointsLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::NoPoints)), Qt::EditRole).toInt()));
			_ui->noPointsLineEdit->blockSignals(false);
		}

		if (column == ult(ImagesLayer::Column::NoDimensions)) {
			_ui->noDimensionsLineEdit->blockSignals(true);
			_ui->noDimensionsLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::NoDimensions)), Qt::EditRole).toInt()));
			_ui->noDimensionsLineEdit->blockSignals(false);
		}

		if (column == ult(LayerNode::Column::SelectionSize)) {
			_ui->selectionSizeLineEdit ->blockSignals(true);
			_ui->selectionSizeLineEdit->setText(QString::number(_layersModel->data(topLeft.siblingAtColumn(ult(LayerNode::Column::SelectionSize)), Qt::EditRole).toInt()));
			_ui->selectionSizeLineEdit->blockSignals(false);
		}

		const auto average				= _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::Average)), Qt::EditRole).toBool();
		const auto averageImagesFlags	= _layersModel->flags(topLeft.siblingAtColumn(ult(ImagesLayer::Column::Average)));

		_ui->currentImageComboBox->setEnabled(mightEdit && !average);
		_ui->currentImageLabel->setEnabled(mightEdit && !average);

		if (column == ult(ImagesLayer::Column::CurrentImageName) || column == ult(ImagesLayer::Column::FilteredImageNames)) {
			
			const auto averageImageNames	= QStringList() << _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::FilteredImageNames)), Qt::DisplayRole).toString();
			const auto imageNames			= _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::FilteredImageNames)), Qt::EditRole).toStringList();

			_ui->currentImageComboBox->blockSignals(true);
			_ui->currentImageComboBox->setModel(new QStringListModel(average ? averageImageNames : imageNames));
			_ui->currentImageComboBox->setCurrentText(_layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::CurrentImageName)), Qt::EditRole).toString());
			_ui->currentImageComboBox->blockSignals(false);
		}

		if (column == ult(ImagesLayer::Column::Average)) {
			const auto averageFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(ImagesLayer::Column::Average)));

			_ui->averageCheckBox->setEnabled(mightEdit && averageFlags & Qt::ItemIsEditable);

			if (column == ult(ImagesLayer::Column::Average)) {
				const auto average = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::Average)), Qt::EditRole).toBool() : false;

				_ui->averageCheckBox->blockSignals(true);
				_ui->averageCheckBox->setChecked(average);
				_ui->averageCheckBox->blockSignals(false);
			}
		}
	}
}