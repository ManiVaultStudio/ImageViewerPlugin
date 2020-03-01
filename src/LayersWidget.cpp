#include "LayersWidget.h"
#include "MainModel.h"
#include "DatasetsModel.h"
#include "LayersModel.h"
#include "ImageDataset.h"

#include "ui_LayersWidget.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QDataWidgetMapper>
#include <QStringListModel>
#include <QDebug>

LayersWidget::LayersWidget(QWidget* parent, MainModel* mainModel) :
	QWidget(parent),
	_mainModel(mainModel),
	_ui{ std::make_unique<Ui::LayersWidget>() }
{
	_ui->setupUi(this);
	
	/*
	auto datasetsModel = _mainModel->datasetsModel();

	_ui->datasetsComboBox->setModel(datasetsModel);
	_ui->datasetsTreeView->setModel(datasetsModel);
	_ui->datasetsTreeView->setSelectionModel(_mainModel->selectionModel());

	QObject::connect(_ui->datasetsComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_mainModel->selectionModel()->select(_mainModel->datasetsModel()->index(currentIndex), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	});

	QObject::connect(_ui->currentImageComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_mainModel->datasetsModel()->setCurrentImage(_mainModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->currentDimensionComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [this](int currentIndex) {
		_mainModel->datasetsModel()->setCurrentDimension(_mainModel->selectionModel()->currentIndex().row(), currentIndex);
	});

	QObject::connect(_ui->averageImagesCheckBox, &QCheckBox::stateChanged, [this](int state) {
		_mainModel->datasetsModel()->setAverageImages(_mainModel->selectionModel()->currentIndex().row(), state);
	});

	QObject::connect(_mainModel->selectionModel(), &QItemSelectionModel::currentRowChanged, _dataWidgetMapper, &QDataWidgetMapper::setCurrentModelIndex);

	QObject::connect(_mainModel->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		_ui->datasetsComboBox->blockSignals(true);
		_ui->datasetsComboBox->setCurrentIndex(current.row());
		_ui->datasetsComboBox->blockSignals(false);

		const auto currentImageFlags = _mainModel->datasetsModel()->flags(_mainModel->datasetsModel()->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentImage)));

		_ui->currentImageComboBox->blockSignals(true);
		_ui->currentImageLabel->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setEnabled(currentImageFlags & Qt::ItemIsEditable);
		_ui->currentImageComboBox->setModel(new QStringListModel(_mainModel->datasetsModel()->imageNames(current.row())));
		_ui->currentImageComboBox->setCurrentIndex(_mainModel->datasetsModel()->currentImage(current.row()));
		_ui->currentImageComboBox->blockSignals(false);

		const auto currentDimensionFlags = _mainModel->datasetsModel()->flags(_mainModel->datasetsModel()->index(current.row(), static_cast<int>(DatasetsModel::Columns::CurrentDimension)));

		_ui->currentDimensionComboBox->blockSignals(true);
		_ui->currentDimensionLabel->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setEnabled(currentDimensionFlags & Qt::ItemIsEditable);
		_ui->currentDimensionComboBox->setModel(new QStringListModel(_mainModel->datasetsModel()->dimensionNames(current.row())));
		_ui->currentDimensionComboBox->setCurrentIndex(_mainModel->datasetsModel()->currentDimension(current.row()));
		_ui->currentDimensionComboBox->blockSignals(false);

		const auto averageImagesFlags = _mainModel->datasetsModel()->flags(_mainModel->datasetsModel()->index(current.row(), static_cast<int>(DatasetsModel::Columns::AverageImages)));

		_ui->averageImagesCheckBox->blockSignals(true);
		_ui->averageImagesCheckBox->setEnabled(averageImagesFlags & Qt::ItemIsEditable);
		_ui->averageImagesCheckBox->setChecked(_mainModel->datasetsModel()->averageImages(current.row()));
		_ui->averageImagesCheckBox->blockSignals(false);
	});
	*/
}

LayersWidget::~LayersWidget() = default;