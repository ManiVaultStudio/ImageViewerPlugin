#include "LayersWidget.h"
#include "LayersModel.h"
#include "DatasetsModel.h"

#include "ui_LayersWidget.h"

#include <QDebug>

LayersWidget::LayersWidget(QWidget* parent, DatasetsModel* datasetsModel) :
	QWidget(parent),
	_datasetsModel(datasetsModel),
	_ui{ std::make_unique<Ui::LayersWidget>() }
{
	_ui->setupUi(this);

	QObject::connect(_datasetsModel->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		//	_ui->layersGroupBox->setEnabled(true);
	});
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::setModel(LayersModel* previous, LayersModel* current)
{
	/*
	auto selectionModel = _layersModel->selectionModel();

	_ui->layersTreeView->setModel(_layersModel);
	_ui->layersTreeView->setSelectionModel(_layersModel->selectionModel());

	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	headerView->hideSection(static_cast<int>(LayersModel::Columns::Order));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Window));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Level));

	QObject::connect(_layersModel->datasetsModel()->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		//	_ui->layersGroupBox->setEnabled(true);
	});

	QObject::connect(_layersModel->datasetsModel()->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		//	_ui->layersGroupBox->setEnabled(_layersModel->datasetsModel()->rowCount(QModelIndex()) > 0);
	});

	QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		auto datasetsModel = _layersModel->datasetsModel();

		const auto opacityFlags = _layersModel->flags(_layersModel->index(current.row(), static_cast<int>(LayersModel::Columns::Opacity)));

		_ui->layerOpacityLabel->setEnabled(opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityDoubleSpinBox->setEnabled(opacityFlags & Qt::ItemIsEditable);
		_ui->layerOpacityHorizontalSlider->setEnabled(opacityFlags & Qt::ItemIsEditable);

		_ui->layerOpacityDoubleSpinBox->blockSignals(true);
		_ui->layerOpacityDoubleSpinBox->setValue(_layersModel->opacity(current.row()));
		_ui->layerOpacityDoubleSpinBox->blockSignals(false);

		_ui->layerOpacityHorizontalSlider->blockSignals(true);
		_ui->layerOpacityHorizontalSlider->setValue(100.0f * _layersModel->opacity(current.row()));
		_ui->layerOpacityHorizontalSlider->blockSignals(false);

		const auto windowFlags = _layersModel->flags(_layersModel->index(current.row(), static_cast<int>(LayersModel::Columns::Window)));
		const auto window = _layersModel->window(current.row());

		_ui->layerWindowLabel->setEnabled(windowFlags & Qt::ItemIsEditable);
		_ui->layerWindowDoubleSpinBox->setEnabled(windowFlags & Qt::ItemIsEditable);
		_ui->layerWindowHorizontalSlider->setEnabled(windowFlags & Qt::ItemIsEditable);

		_ui->layerWindowDoubleSpinBox->blockSignals(true);
		_ui->layerWindowDoubleSpinBox->setValue(window);
		_ui->layerWindowDoubleSpinBox->blockSignals(false);

		_ui->layerWindowHorizontalSlider->blockSignals(true);
		_ui->layerWindowHorizontalSlider->setValue(100.0f * window);
		_ui->layerWindowHorizontalSlider->blockSignals(false);

		const auto levelFlags = _layersModel->flags(_layersModel->index(current.row(), static_cast<int>(LayersModel::Columns::Level)));
		const auto level = _layersModel->level(current.row());

		_ui->layerLevelLabel->setEnabled(levelFlags & Qt::ItemIsEditable);
		_ui->layerLevelDoubleSpinBox->setEnabled(levelFlags & Qt::ItemIsEditable);
		_ui->layerLevelHorizontalSlider->setEnabled(levelFlags & Qt::ItemIsEditable);

		_ui->layerLevelDoubleSpinBox->blockSignals(true);
		_ui->layerLevelDoubleSpinBox->setValue(level);
		_ui->layerLevelDoubleSpinBox->blockSignals(false);

		_ui->layerLevelHorizontalSlider->blockSignals(true);
		_ui->layerLevelHorizontalSlider->setValue(100.0f * level);
		_ui->layerLevelHorizontalSlider->blockSignals(false);
	});
	*/
}