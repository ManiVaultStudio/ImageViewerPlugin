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

	QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
		_layersModel->removeRows(_ui->layersTreeView->selectionModel()->selectedRows());
	});

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int enabled) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Enabled), enabled);
		}
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = _layersModel->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), LayersModel::Columns::Opacity);
		const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();

		_layersModel->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Opacity), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Window), value);
		}
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Window), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Level), value);
		}
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();
			_layersModel->setData(_layersModel->index(index.row(), LayersModel::Columns::Level), value / static_cast<float>(range));
		}
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
	headerView->hideSection(LayersModel::Columns::Removable);
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

	auto updateButtons = [this]() {
		const auto selectedRows		= _ui->layersTreeView->selectionModel()->selectedRows();
		const auto noSelectedRows	= selectedRows.size();

		_ui->layerRemovePushButton->setEnabled(false);
		_ui->layerMoveUpPushButton->setEnabled(false);
		_ui->layerMoveDownPushButton->setEnabled(false);

		_ui->layerRemovePushButton->setToolTip("");
		_ui->layerMoveUpPushButton->setToolTip("");
		_ui->layerMoveDownPushButton->setToolTip("");

		if (noSelectedRows == 1) {
			const auto row			= selectedRows.at(0).row();
			const auto name			= _layersModel->data(row, LayersModel::Columns::Name, Qt::EditRole).toString();
			const auto mayRemove	= _layersModel->data(row, LayersModel::Columns::Removable, Qt::EditRole).toBool();

			_ui->layerRemovePushButton->setEnabled(mayRemove);
			_ui->layerRemovePushButton->setToolTip(mayRemove ? QString("Remove %1").arg(name) : "");

			const auto mayMoveUp = _layersModel->mayMoveUp(row);

			_ui->layerMoveUpPushButton->setEnabled(mayMoveUp);
			_ui->layerMoveUpPushButton->setToolTip(mayMoveUp ? QString("Move %1 up one level").arg(name) : "");

			const auto mayMoveDown = _layersModel->mayMoveDown(row);

			_ui->layerMoveDownPushButton->setEnabled(mayMoveDown);
			_ui->layerMoveDownPushButton->setToolTip(mayMoveDown ? QString("Move %1 down one level").arg(name) : "");
		}

		if (noSelectedRows >= 1) {
			auto names		= QStringList();
			auto mayRemove	= false;

			for (auto index : selectedRows) {
				if (_layersModel->data(index.row(), LayersModel::Columns::Removable, Qt::EditRole).toBool()) {
					mayRemove = true;
					names << _layersModel->data(index.row(), LayersModel::Columns::Name, Qt::EditRole).toString();
				}
			}
			
			if (mayRemove) {
				_ui->layerRemovePushButton->setEnabled(true);
				_ui->layerRemovePushButton->setToolTip(QString("Remove %1").arg(names.join(", ")));
			}
		}
	};

	// Handle user row selection
	QObject::connect(_ui->layersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged, [this, updateButtons](const QItemSelection &selected, const QItemSelection &deselected) {
		updateButtons();
		
		const auto selectedRows = _ui->layersTreeView->selectionModel()->selectedRows();

		if (selectedRows.size() >= 1)
			updateData(_layersModel->index(selectedRows.first().row(), 0), _layersModel->index(selectedRows.first().row(), _layersModel->columnCount() - 1));
	});

	// Handle model rows reorganization
	QObject::connect(_layersModel.get(), &QAbstractListModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateButtons();
	});

	QObject::connect(_layersModel.get(), &LayersModel::dataChanged, this, &LayersWidget::updateData);

	_ui->layersTreeView->selectionModel()->setCurrentIndex(_layersModel->index(0), QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void LayersWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	const auto selectedRows		= _ui->layersTreeView->selectionModel()->selectedRows();
	const auto noSelectedRows	= selectedRows.size();

	const auto enabledFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Enabled));
	const auto enabled		= noSelectedRows == 1 && _layersModel->data(topLeft.row(), LayersModel::Columns::Enabled, Qt::EditRole).toBool();

	if (topLeft.column() <= LayersModel::Columns::Enabled && bottomRight.column() >= LayersModel::Columns::Enabled) {
		_ui->layerEnabledCheckBox->setEnabled(enabled && enabledFlags & Qt::ItemIsEditable);

		_ui->layerEnabledCheckBox->blockSignals(true);
		_ui->layerEnabledCheckBox->setChecked(enabled);
		_ui->layerEnabledCheckBox->blockSignals(false);
	}

	const auto opacityFlags = _layersModel->flags(_layersModel->index(topLeft.row(), LayersModel::Columns::Opacity));

	_ui->layerOpacityLabel->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityDoubleSpinBox->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityHorizontalSlider->setEnabled(enabled && opacityFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= LayersModel::Columns::Opacity && bottomRight.column() >= LayersModel::Columns::Opacity) {
		const auto opacity = _layersModel->data(topLeft.row(), LayersModel::Columns::Opacity, Qt::EditRole).toFloat();

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
		const auto window = _layersModel->data(topLeft.row(), LayersModel::Columns::Window, Qt::EditRole).toFloat();

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
		const auto level = _layersModel->data(topLeft.row(), LayersModel::Columns::Level, Qt::EditRole).toFloat();

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
		const auto color = _layersModel->data(topLeft.row(), LayersModel::Columns::Color, Qt::EditRole).value<QColor>();

		_ui->layerColorComboBox->blockSignals(true);
		//_ui->layerColorComboBox->setChecked(enabled);
		_ui->layerColorComboBox->blockSignals(false);
	}
}