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
		setModel(_datasetsModel->datasets()[current.row()]->_layersModel);
	});

	setModel(QSharedPointer<LayersModel>());

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		_layersModel->moveUp(_ui->layersTreeView->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		_layersModel->moveDown(_ui->layersTreeView->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int enabled) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Enabled);
		_layersModel->setData(index, enabled);
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Opacity);
		const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Opacity);
		const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Window);
		_layersModel->setData(index, value);
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Window);
		const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Level);
		_layersModel->setData(index, value);
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Level);
		const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::setModel(QSharedPointer<LayersModel> layersModel)
{
	_layersModel = layersModel;

	if (_layersModel == nullptr) {
		_ui->layersGroupBox->setEnabled(false);
		return;
	}
	
	_ui->layersGroupBox->setEnabled(true);
	_ui->layersTreeView->setModel(layersModel.get());

	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	//headerView->hideSection(LayersModel::Columns::Name);
	//headerView->hideSection(LayersModel::Columns::Type);
	headerView->hideSection(LayersModel::Columns::Enabled);
	headerView->hideSection(LayersModel::Columns::Fixed);
	headerView->hideSection(LayersModel::Columns::Order);
	//headerView->hideSection(LayersModel::Columns::Opacity);
	//headerView->hideSection(LayersModel::Columns::Window);
	//headerView->hideSection(LayersModel::Columns::Level);
	headerView->hideSection(LayersModel::Columns::Color);

	const QStringList colorNames = QColor::colorNames();
	int index = 0;

	for (const auto& colorName : colorNames) {
		const QColor color(colorName);
		_ui->layerColorComboBox->addItem(colorName, color);
		const QModelIndex idx = _ui->layerColorComboBox->model()->index(index++, 0);
		_ui->layerColorComboBox->model()->setData(idx, color, Qt::DecorationRole);
	}

	// Update the buttons that re-organize rows
	auto updateOrderPushButtons = [this](const int& row) {
		_ui->layerMoveUpPushButton->setEnabled(_layersModel->mayMoveUp(row));
		_ui->layerMoveDownPushButton->setEnabled(_layersModel->mayMoveDown(row));
	};

	// Handle user row selection
	QObject::connect(_ui->layersTreeView->selectionModel(), &QItemSelectionModel::currentRowChanged, [this, updateOrderPushButtons](const QModelIndex& current, const QModelIndex& previous) {
		updateOrderPushButtons(current.row());
		updateData(_layersModel->index(current.row(), 0), _layersModel->index(current.row(), _layersModel->columnCount() - 1));
	});

	// Handle model rows reorganization
	QObject::connect(_layersModel.get(), &QAbstractListModel::rowsMoved, [this, updateOrderPushButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateOrderPushButtons(_ui->layersTreeView->selectionModel()->currentIndex().row());
	});

	QObject::connect(_layersModel.get(), &LayersModel::dataChanged, this, &LayersWidget::updateData);

	_ui->layersTreeView->selectionModel()->setCurrentIndex(_layersModel->index(0), QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void LayersWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	const auto enabledFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Enabled));
	const auto enabled = _layersModel->enabled(topLeft.row(), Qt::EditRole).toBool();

	if (topLeft.column() <= LayersModel::Columns::Enabled && bottomRight.column() >= LayersModel::Columns::Enabled) {
		_ui->layerEnabledCheckBox->setEnabled(enabledFlags & Qt::ItemIsEditable);

		_ui->layerEnabledCheckBox->blockSignals(true);
		_ui->layerEnabledCheckBox->setChecked(enabled);
		_ui->layerEnabledCheckBox->blockSignals(false);
	}

	const auto opacityFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Opacity));

	_ui->layerOpacityLabel->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityDoubleSpinBox->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityHorizontalSlider->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= LayersModel::Columns::Opacity && bottomRight.column() >= LayersModel::Columns::Opacity) {
		const auto opacity = _layersModel->opacity(topLeft.row(), Qt::EditRole).toFloat();

		_ui->layerOpacityDoubleSpinBox->blockSignals(true);
		_ui->layerOpacityDoubleSpinBox->setValue(100.0f * opacity);
		_ui->layerOpacityDoubleSpinBox->blockSignals(false);

		_ui->layerOpacityHorizontalSlider->blockSignals(true);
		_ui->layerOpacityHorizontalSlider->setValue(100.0f * opacity);
		_ui->layerOpacityHorizontalSlider->blockSignals(false);
	}

	const auto windowFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Window));

	_ui->layerWindowLabel->setEnabled(enabled && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowDoubleSpinBox->setEnabled(enabled && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowHorizontalSlider->setEnabled(enabled && windowFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= LayersModel::Columns::Window && bottomRight.column() >= LayersModel::Columns::Window) {
		const auto window = _layersModel->window(topLeft.row(), Qt::EditRole).toFloat();

		_ui->layerWindowDoubleSpinBox->blockSignals(true);
		_ui->layerWindowDoubleSpinBox->setValue(window);
		_ui->layerWindowDoubleSpinBox->blockSignals(false);

		_ui->layerWindowHorizontalSlider->blockSignals(true);
		_ui->layerWindowHorizontalSlider->setValue(100.0f * window);
		_ui->layerWindowHorizontalSlider->blockSignals(false);
	}

	const auto levelFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Level));

	_ui->layerLevelLabel->setEnabled(enabled && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelDoubleSpinBox->setEnabled(enabled && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelHorizontalSlider->setEnabled(enabled && levelFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= LayersModel::Columns::Level && bottomRight.column() >= LayersModel::Columns::Level) {
		const auto level = _layersModel->level(topLeft.row(), Qt::EditRole).toFloat();

		_ui->layerLevelDoubleSpinBox->blockSignals(true);
		_ui->layerLevelDoubleSpinBox->setValue(level);
		_ui->layerLevelDoubleSpinBox->blockSignals(false);

		_ui->layerLevelHorizontalSlider->blockSignals(true);
		_ui->layerLevelHorizontalSlider->setValue(100.0f * level);
		_ui->layerLevelHorizontalSlider->blockSignals(false);
	}

	const auto colorFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Color));

	_ui->layerColorLabel->setEnabled(enabled && colorFlags & Qt::ItemIsEditable);
	_ui->layerColorComboBox->setEnabled(enabled && colorFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= LayersModel::Columns::Color && bottomRight.column() >= LayersModel::Columns::Color) {
		const auto color = _layersModel->enabled(topLeft.row(), Qt::EditRole).value<QColor>();

		_ui->layerColorComboBox->blockSignals(true);
		//_ui->layerColorComboBox->setChecked(enabled);
		_ui->layerColorComboBox->blockSignals(false);
	}
}