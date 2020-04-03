#include "ImagesLayerWidget.h"
#include "LayersModel.h"
#include "ImagesLayer.h"

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

void ImagesLayerWidget::initialize(LayersModel* layersModel)
{
	_layersModel = layersModel;

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImagesLayerWidget::updateData);

	QObject::connect(&_layersModel->selectionModel(), &QItemSelectionModel::selectionChanged, [this](const QItemSelection& selected, const QItemSelection& deselected) {
		const auto selectedRows = _layersModel->selectionModel().selectedRows();

		if (selectedRows.isEmpty())
			updateData(QModelIndex(), QModelIndex());
		else {
			const auto first = selected.indexes().first();
			updateData(first.siblingAtColumn(ult(ImagesLayer::Column::Start)), first.siblingAtColumn(ult(ImagesLayer::Column::End)));
		}
	});

	QObject::connect(_ui->imagesComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().row(), ult(ImagesLayer::Column::CurrentImageId), currentIndex);
	});

	QObject::connect(_ui->averageCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_layersModel->setData(_layersModel->selectionModel().currentIndex().row(), ult(ImagesLayer::Column::Average), state);
	});
}

void ImagesLayerWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _layersModel->selectionModel().selectedRows();
	const auto noSelectedRows	= selectedRows.size();
	const auto enabled			= _layersModel->data(topLeft.siblingAtColumn(ult(Layer::Column::Name)), Qt::CheckStateRole).toInt() == Qt::Checked;

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
		
		if (column == ult(ImagesLayer::Column::ImageSize)) {
			const auto imageSize = _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::ImageSize)), Qt::EditRole).toSize();

			_ui->imageSizeLineEdit->blockSignals(true);
			_ui->imageSizeLineEdit->setText(QString("[%1, %2]").arg(QString::number(imageSize.width()), QString::number(imageSize.height())));
			_ui->imageSizeLineEdit->blockSignals(false);
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

		const auto filteredImageNamesFlags = _layersModel->flags(topLeft.siblingAtColumn(ult(ImagesLayer::Column::FilteredImageNames)));

		_ui->imagesComboBox->setEnabled(mightEdit && filteredImageNamesFlags & Qt::ItemIsEditable);
		_ui->imagesLabel->setEnabled(mightEdit && filteredImageNamesFlags & Qt::ItemIsEditable);

		if (column == ult(ImagesLayer::Column::FilteredImageNames)) {
			const auto filteredImageNames = validSelection ? _layersModel->data(topLeft.siblingAtColumn(ult(ImagesLayer::Column::FilteredImageNames)), Qt::EditRole).toStringList() : QStringList();

			_ui->imagesComboBox->blockSignals(true);
			_ui->imagesComboBox->setModel(new QStringListModel(filteredImageNames));
			_ui->imagesComboBox->blockSignals(false);
		}

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