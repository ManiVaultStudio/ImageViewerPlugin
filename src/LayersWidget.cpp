#include "LayersWidget.h"
#include "LayersModel.h"
#include "DatasetsModel.h"

#include "ui_LayersWidget.h"

#include <QSortFilterProxyModel>
#include <QDebug>

LayersWidget::LayersWidget(QWidget* parent, DatasetsModel* datasetsModel) :
	QWidget(parent),
	_datasetsModel(datasetsModel),
	_layersModel(),
	_ui{ std::make_unique<Ui::LayersWidget>() }
{
	_ui->setupUi(this);

	QObject::connect(_datasetsModel->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		setModel(_datasetsModel->layersModel(current.row()));
	});

	setModel(QSharedPointer<LayersModel>(nullptr));
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::setModel(QSharedPointer<LayersModel> layersModel)
{
	_layersModel = layersModel;

	if (_layersModel.isNull()) {
		_ui->layersGroupBox->setEnabled(false);
		return;
	}

	_ui->layersGroupBox->setEnabled(true);

	auto selectionModel = layersModel->selectionModel();

	_ui->layersTreeView->setModel(layersModel.get());
	_ui->layersTreeView->setSelectionModel(_layersModel->selectionModel());

	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	//headerView->hideSection(static_cast<int>(LayersModel::Columns::Name));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Type));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Enabled));
	//headerView->hideSection(static_cast<int>(LayersModel::Columns::Order));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Window));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Level));
	headerView->hideSection(static_cast<int>(LayersModel::Columns::Color));

	const QStringList colorNames = QColor::colorNames();
	int index = 0;

	for (const auto& colorName : colorNames) {
		const QColor color(colorName);
		_ui->layerColorComboBox->addItem(colorName, color);
		const QModelIndex idx = _ui->layerColorComboBox->model()->index(index++, 0);
		_ui->layerColorComboBox->model()->setData(idx, color, Qt::DecorationRole);
	}

	QObject::connect(selectionModel, &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		const auto topLeft		= _layersModel->index(current.row(), 0);
		const auto bottomRight	= _layersModel->index(current.row(), _layersModel->rowCount(QModelIndex()) - 1);

		//updateData(topLeft, bottomRight);

		const auto order = _layersModel->order(current.row(), Qt::EditRole);

		_ui->layerMoveUpPushButton->setEnabled(order >= 1);
		_ui->layerMoveDownPushButton->setEnabled(order < _layersModel->rowCount() - 1);
	});

	QObject::connect(_layersModel.get(), &LayersModel::dataChanged, this, &LayersWidget::updateData);

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		const auto currentRow = _layersModel->selectionModel()->currentIndex().row();
		_layersModel->moveUp(currentRow);

		//_layersModel->moveRows(QModelIndex(), currentRow, 1, QModelIndex(), currentRow - 1);
		//_layersModel->selectionModel()->select(_layersModel->index(currentRow - 1), QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		_layersModel->moveDown(_layersModel->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int enabled) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Enabled));
		_layersModel->setData(index, enabled);
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Opacity));
		_layersModel->setData(index, value);
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Opacity));
		const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Window));
		_layersModel->setData(index, value);
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Window));
		const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Level));
		_layersModel->setData(index, value);
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_layersModel->selectionModel()->currentIndex().row(), static_cast<int>(LayersModel::Columns::Level));
		const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});
}

void LayersWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	

	const auto enabledFlags = _layersModel->flags(_layersModel->index(topLeft.row(), static_cast<int>(LayersModel::Columns::Enabled)));
	const auto enabled		= _layersModel->enabled(topLeft.row(), Qt::EditRole).toBool();

	_ui->layerEnabledCheckBox->setEnabled(enabledFlags & Qt::ItemIsEditable);

	_ui->layerEnabledCheckBox->blockSignals(true);
	_ui->layerEnabledCheckBox->setChecked(enabled);
	_ui->layerEnabledCheckBox->blockSignals(false);

	const auto opacityFlags = _layersModel->flags(_layersModel->index(topLeft.row(), static_cast<int>(LayersModel::Columns::Opacity)));
	const auto opacity = _layersModel->opacity(topLeft.row(), Qt::EditRole).toFloat();

	_ui->layerOpacityLabel->setEnabled(opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityDoubleSpinBox->setEnabled(opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityHorizontalSlider->setEnabled(opacityFlags & Qt::ItemIsEditable);

	_ui->layerOpacityDoubleSpinBox->blockSignals(true);
	_ui->layerOpacityDoubleSpinBox->setValue(opacity);
	_ui->layerOpacityDoubleSpinBox->blockSignals(false);

	_ui->layerOpacityHorizontalSlider->blockSignals(true);
	_ui->layerOpacityHorizontalSlider->setValue(100.0f * opacity);
	_ui->layerOpacityHorizontalSlider->blockSignals(false);

	const auto windowFlags = _layersModel->flags(_layersModel->index(topLeft.row(), static_cast<int>(LayersModel::Columns::Window)));
	const auto window = _layersModel->window(topLeft.row(), Qt::EditRole).toFloat();

	_ui->layerWindowLabel->setEnabled(windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowDoubleSpinBox->setEnabled(windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowHorizontalSlider->setEnabled(windowFlags & Qt::ItemIsEditable);

	_ui->layerWindowDoubleSpinBox->blockSignals(true);
	_ui->layerWindowDoubleSpinBox->setValue(window);
	_ui->layerWindowDoubleSpinBox->blockSignals(false);

	_ui->layerWindowHorizontalSlider->blockSignals(true);
	_ui->layerWindowHorizontalSlider->setValue(100.0f * window);
	_ui->layerWindowHorizontalSlider->blockSignals(false);

	const auto levelFlags = _layersModel->flags(_layersModel->index(topLeft.row(), static_cast<int>(LayersModel::Columns::Level)));
	const auto level = _layersModel->level(topLeft.row(), Qt::EditRole).toFloat();

	_ui->layerLevelLabel->setEnabled(levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelDoubleSpinBox->setEnabled(levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelHorizontalSlider->setEnabled(levelFlags & Qt::ItemIsEditable);

	_ui->layerLevelDoubleSpinBox->blockSignals(true);
	_ui->layerLevelDoubleSpinBox->setValue(level);
	_ui->layerLevelDoubleSpinBox->blockSignals(false);

	_ui->layerLevelHorizontalSlider->blockSignals(true);
	_ui->layerLevelHorizontalSlider->setValue(100.0f * level);
	_ui->layerLevelHorizontalSlider->blockSignals(false);

	const auto colorFlags	= _layersModel->flags(_layersModel->index(topLeft.row(), static_cast<int>(LayersModel::Columns::Color)));
	const auto color		= _layersModel->enabled(topLeft.row(), Qt::EditRole).value<QColor>();

	

	_ui->layerColorLabel->setEnabled(colorFlags & Qt::ItemIsEditable);
	_ui->layerColorComboBox->setEnabled(colorFlags & Qt::ItemIsEditable);

	_ui->layerColorComboBox->blockSignals(true);
	//_ui->layerColorComboBox->setChecked(enabled);
	_ui->layerColorComboBox->blockSignals(false);
}