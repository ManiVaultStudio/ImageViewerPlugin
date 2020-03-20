#include "LayersWidget.h"
#include "ImageViewerPlugin.h"
#include "LayersModel.h"
#include "PointsLayer.h"
#include "ImagesLayer.h"

#include "ui_LayersWidget.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QItemSelectionModel>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>

LayersWidget::LayersWidget(ImageViewerPlugin* imageViewerPlugin) :
	QWidget(),
	_imageViewerPlugin(imageViewerPlugin),
	_ui{ std::make_unique<Ui::LayersWidget>() }
{
	setAcceptDrops(true);

	_ui->setupUi(this);
	
	/*
	QObject::connect(_datasetsModel->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		setModel(_datasetsModel->datasets()[current.row()]->layersModel());
	});
	*/

	_ui->datasetsTreeView->setModel(&_imageViewerPlugin->datasetsModel());

	_ui->maskCheckBox->setVisible(false);

	QFont font = QFont("Font Awesome 5 Free Solid", 9);

	_ui->layerRemovePushButton->setFont(font);
	_ui->layerMoveUpPushButton->setFont(font);
	_ui->layerMoveDownPushButton->setFont(font);

	_ui->layerRemovePushButton->setText(u8"\uf2ed");
	_ui->layerMoveUpPushButton->setText(u8"\uf0d8");
	_ui->layerMoveDownPushButton->setText(u8"\uf0d7");

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		layersModel()->moveUp(_ui->layersTreeView->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		layersModel()->moveDown(_ui->layersTreeView->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
		layersModel()->removeRows(_ui->layersTreeView->selectionModel()->selectedRows());
	});

	QObject::connect(_ui->layerNameLineEdit, &QLineEdit::textEdited, [this](const QString& text) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::Name)), text);
		}
	});

	QObject::connect(_ui->layerEnabledCheckBox, &QCheckBox::stateChanged, [this](int state) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::Enabled)), static_cast<int>(state), Qt::CheckStateRole);
		}
	});

	QObject::connect(_ui->layerOpacityDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		const auto index = layersModel()->index(_ui->layersTreeView->selectionModel()->currentIndex().row(), layerColumnId(Layer::Column::Opacity));
		const auto range = _ui->layerOpacityDoubleSpinBox->maximum() - _ui->layerOpacityDoubleSpinBox->minimum();

		layersModel()->setData(index, value / static_cast<float>(range));
	});

	QObject::connect(_ui->layerOpacityHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerOpacityHorizontalSlider->maximum() - _ui->layerOpacityHorizontalSlider->minimum();
			
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::Opacity)), static_cast<float>(value) / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerWindowDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::WindowNormalized)), value);
		}
	});

	QObject::connect(_ui->layerWindowHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerWindowHorizontalSlider->maximum() - _ui->layerWindowHorizontalSlider->minimum();
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::WindowNormalized)), value / static_cast<float>(range));
		}
	});

	QObject::connect(_ui->layerLevelDoubleSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::LevelNormalized)), value);
		}
	});

	QObject::connect(_ui->layerLevelHorizontalSlider, &QSlider::valueChanged, [this](int value) {
		for (auto index : _ui->layersTreeView->selectionModel()->selectedRows()) {
			const auto range = _ui->layerLevelHorizontalSlider->maximum() - _ui->layerLevelHorizontalSlider->minimum();
			layersModel()->setData(layersModel()->index(index.row(), layerColumnId(Layer::Column::LevelNormalized)), value / static_cast<float>(range));
		}
	});

	_ui->layersGroupBox->setEnabled(true);
	_ui->layersTreeView->setModel(layersModel());

	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	//headerView->hideSection(layerColumnId(Layer::Column::Type)));
	//headerView->hideSection(layerColumnId(Layer::Column::Enabled));
	headerView->hideSection(layerColumnId(Layer::Column::Locked));
	headerView->hideSection(layerColumnId(Layer::Column::ID));
	//headerView->hideSection(layerColumnId(Layer::Column::Name));
	//headerView->hideSection(layerColumnId(Layer::Column::Dataset));
	headerView->hideSection(layerColumnId(Layer::Column::Flags));
	headerView->hideSection(layerColumnId(Layer::Column::Frozen));
	headerView->hideSection(layerColumnId(Layer::Column::Removable));
	headerView->hideSection(layerColumnId(Layer::Column::Mask));
	headerView->hideSection(layerColumnId(Layer::Column::Renamable));
	headerView->hideSection(layerColumnId(Layer::Column::Order));
	//headerView->hideSection(layerColumnId(Layer::Column::Opacity));
	headerView->hideSection(layerColumnId(Layer::Column::WindowNormalized));
	headerView->hideSection(layerColumnId(Layer::Column::LevelNormalized));
	headerView->hideSection(layerColumnId(Layer::Column::ColorMap));
	headerView->hideSection(layerColumnId(Layer::Column::Image));
	headerView->hideSection(layerColumnId(Layer::Column::ImageRange));
	headerView->hideSection(layerColumnId(Layer::Column::DisplayRange));

	headerView->setSectionResizeMode(layerColumnId(Layer::Column::Name), QHeaderView::Interactive);

	auto updateButtons = [this]() {
		const auto selectedRows = _ui->layersTreeView->selectionModel()->selectedRows();
		const auto noSelectedRows = selectedRows.size();

		_ui->layerRemovePushButton->setEnabled(false);
		_ui->layerMoveUpPushButton->setEnabled(false);
		_ui->layerMoveDownPushButton->setEnabled(false);

		_ui->layerRemovePushButton->setToolTip("");
		_ui->layerMoveUpPushButton->setToolTip("");
		_ui->layerMoveDownPushButton->setToolTip("");

		if (noSelectedRows == 1) {
			const auto row = selectedRows.at(0).row();
			const auto name = layersModel()->data(row, layerColumnId(Layer::Column::Name), Qt::EditRole).toString();
			const auto mayRemove = layersModel()->data(row, layerColumnId(Layer::Column::Removable), Qt::EditRole).toBool();

			_ui->layerRemovePushButton->setEnabled(mayRemove);
			_ui->layerRemovePushButton->setToolTip(mayRemove ? QString("Remove %1").arg(name) : "");

			const auto mayMoveUp = layersModel()->mayMoveUp(row);

			_ui->layerMoveUpPushButton->setEnabled(mayMoveUp);
			_ui->layerMoveUpPushButton->setToolTip(mayMoveUp ? QString("Move %1 up one level").arg(name) : "");

			const auto mayMoveDown = layersModel()->mayMoveDown(row);

			_ui->layerMoveDownPushButton->setEnabled(mayMoveDown);
			_ui->layerMoveDownPushButton->setToolTip(mayMoveDown ? QString("Move %1 down one level").arg(name) : "");
		}

		if (noSelectedRows >= 1) {
			auto names = QStringList();
			auto mayRemove = false;

			for (auto index : selectedRows) {
				if (layersModel()->data(index.row(), layerColumnId(Layer::Column::Removable), Qt::EditRole).toBool()) {
					mayRemove = true;
					names << layersModel()->data(index.row(), layerColumnId(Layer::Column::Name), Qt::EditRole).toString();
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

		if (selectedRows.isEmpty())
			updateData(layersModel()->index(0, 0), layersModel()->index(0, layersModel()->columnCount() - 1));
		else
			updateData(layersModel()->index(selectedRows.first().row(), 0), layersModel()->index(selectedRows.first().row(), layersModel()->columnCount() - 1));
	});

	// Handle model rows reorganization
	QObject::connect(layersModel(), &QAbstractListModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateButtons();
	});

	QObject::connect(layersModel(), &LayersModel::dataChanged, this, &LayersWidget::updateData);

	_ui->layersTreeView->selectionModel()->setCurrentIndex(layersModel()->index(0), QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/)
{
	const auto selectedRows			= _ui->layersTreeView->selectionModel()->selectedRows();
	const auto noSelectedRows		= selectedRows.size();
	const auto singleRowSelection	= noSelectedRows == 1;
	const auto enabled				= layersModel()->data(topLeft.row(), layerColumnId(Layer::Column::Enabled), Qt::EditRole).toBool();
	const auto mightEdit			= singleRowSelection && enabled;

	const auto enabledFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::Enabled)));

	if (topLeft.column() <= layerColumnId(Layer::Column::Enabled) && bottomRight.column() >= layerColumnId(Layer::Column::Enabled)) {
		_ui->layerEnabledCheckBox->setEnabled(singleRowSelection && enabledFlags & Qt::ItemIsEditable);
		_ui->layerEnabledCheckBox->blockSignals(true);
		_ui->layerEnabledCheckBox->setChecked(singleRowSelection ? enabled : false);
		_ui->layerEnabledCheckBox->blockSignals(false);
	}

	const auto nameFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::Name)));

	_ui->layerNameLabel->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);
	_ui->layerNameLineEdit->setEnabled(mightEdit && nameFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= layerColumnId(Layer::Column::Name) && bottomRight.column() >= layerColumnId(Layer::Column::Name)) {
		const auto name = singleRowSelection ? layersModel()->data(topLeft.row(), layerColumnId(Layer::Column::Name), Qt::EditRole).toString() : "";

		if (name != _ui->layerNameLineEdit->text()) {
			_ui->layerNameLineEdit->blockSignals(true);
			_ui->layerNameLineEdit->setText(name);
			_ui->layerNameLineEdit->blockSignals(false);
		}
	}

	const auto opacityFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::Opacity)));

	_ui->layerOpacityLabel->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityDoubleSpinBox->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);
	_ui->layerOpacityHorizontalSlider->setEnabled(mightEdit && opacityFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= layerColumnId(Layer::Column::Opacity) && bottomRight.column() >= layerColumnId(Layer::Column::Opacity)) {
		const auto opacity = layersModel()->data(topLeft.row(), layerColumnId(Layer::Column::Opacity), Qt::EditRole).toFloat();

		_ui->layerOpacityDoubleSpinBox->blockSignals(true);
		_ui->layerOpacityDoubleSpinBox->setValue(singleRowSelection ? 100.0f * opacity : 100.0f);
		_ui->layerOpacityDoubleSpinBox->blockSignals(false);

		_ui->layerOpacityHorizontalSlider->blockSignals(true);
		_ui->layerOpacityHorizontalSlider->setValue(singleRowSelection ? 100.0f * opacity : 100.0f);
		_ui->layerOpacityHorizontalSlider->blockSignals(false);
	}

	const auto windowFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::WindowNormalized)));

	_ui->layerWindowLabel->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowDoubleSpinBox->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);
	_ui->layerWindowHorizontalSlider->setEnabled(mightEdit && windowFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= layerColumnId(Layer::Column::WindowNormalized) && bottomRight.column() >= layerColumnId(Layer::Column::WindowNormalized)) {
		const auto window = layersModel()->data(topLeft.row(), layerColumnId(Layer::Column::WindowNormalized), Qt::EditRole).toFloat();

		_ui->layerWindowDoubleSpinBox->blockSignals(true);
		_ui->layerWindowDoubleSpinBox->setValue(singleRowSelection ? window : 1.0f);
		_ui->layerWindowDoubleSpinBox->blockSignals(false);

		_ui->layerWindowHorizontalSlider->blockSignals(true);
		_ui->layerWindowHorizontalSlider->setValue(singleRowSelection ? 100.0f * window : 100.0f);
		_ui->layerWindowHorizontalSlider->blockSignals(false);
	}

	const auto levelFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::LevelNormalized)));

	_ui->layerLevelLabel->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelDoubleSpinBox->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);
	_ui->layerLevelHorizontalSlider->setEnabled(mightEdit && levelFlags & Qt::ItemIsEditable);

	if (topLeft.column() <= layerColumnId(Layer::Column::LevelNormalized) && bottomRight.column() >= layerColumnId(Layer::Column::LevelNormalized)) {
		const auto level = layersModel()->data(topLeft.row(), layerColumnId(Layer::Column::LevelNormalized), Qt::EditRole).toFloat();

		_ui->layerLevelDoubleSpinBox->blockSignals(true);
		_ui->layerLevelDoubleSpinBox->setValue(singleRowSelection ? level : 0.5f);
		_ui->layerLevelDoubleSpinBox->blockSignals(false);

		_ui->layerLevelHorizontalSlider->blockSignals(true);
		_ui->layerLevelHorizontalSlider->setValue(singleRowSelection ? 100.0f * level : 50.0f);
		_ui->layerLevelHorizontalSlider->blockSignals(false);
	}

	const auto maskFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::Mask)));

	_ui->maskCheckBox->setEnabled(mightEdit && maskFlags & Qt::ItemIsEditable);

	const auto colorFlags = layersModel()->flags(layersModel()->index(topLeft.row(), layerColumnId(Layer::Column::ColorMap)));
}

void LayersWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items = dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetName = items.at(0);
	const auto datasetType = items.at(1);
	
	/*
	if (layersModel()->findLayerById(datasetName) != nullptr)
		return;
	*/

	if (datasetType == "Points") {
		const auto points		= _imageViewerPlugin->requestData<Points>(datasetName);
		const auto noDimensions	= points.getNumDimensions();

		if (noDimensions >= 1 && noDimensions <= 3)
			dragEnterEvent->acceptProposedAction();
	}

	if (datasetType == "Images" && !layersModel()->findLayerById(datasetName))
		dragEnterEvent->acceptProposedAction();

	if (datasetType == "Clusters")
		dragEnterEvent->acceptProposedAction();
}

void LayersWidget::dropEvent(QDropEvent* dropEvent)
{
	const auto items					= dropEvent->mimeData()->text().split("\n");
	const auto datasetName				= items.at(0);
	const auto datasetType				= items.at(1);
	const auto selectionName			= QString("%1_selection").arg(datasetName);
	const auto createSelectionLayer		= layersModel()->findLayerById(selectionName) == nullptr;
	const auto layerFlags				= static_cast<int>(Layer::Flag::Enabled) | static_cast<int>(Layer::Flag::Removable);

	if (datasetType == "Points") {
		const auto points = _imageViewerPlugin->requestData<Points>(datasetName);

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Points);
		
		switch (points.getNumDimensions())
		{
			case 1:
				layersModel()->addLayer(new PointsLayer(dataset, datasetName, datasetName, layerFlags));
				break;

			case 2:
				layersModel()->addLayer(new PointsLayer(dataset, datasetName, datasetName, layerFlags));
				break;

			case 3:
				layersModel()->addLayer(new PointsLayer(dataset, datasetName, datasetName, layerFlags));
				break;

			default:
				break;
		}

		if (createSelectionLayer)
			layersModel()->addLayer(new Layer(dataset, selectionName, selectionName, Layer::Type::Selection, layerFlags));

		dataset->init();
	}

	if (datasetType == "Images") {
		const auto imagesName		= datasetName;
		const auto selectionName	= QString("%1_selection").arg(datasetName);

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Images);

		layersModel()->addLayer(new ImagesLayer(dataset, imagesName, imagesName, layerFlags));

		if (createSelectionLayer)
			layersModel()->addLayer(new Layer(dataset, selectionName, selectionName, Layer::Type::Selection, layerFlags));

		dataset->init();
	}

	if (datasetType == "Clusters") {

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Clusters);

		/*
		const auto selectedRows = _datasetsModel->selectionModel()->selectedRows();

		if (selectedRows.size() == 1) {
			const auto row = selectedRows.first().row();
			const auto size = _datasetsModel->data(row, DatasetsModel::Columns::Size, Qt::EditRole).toSize();
			const auto clusters = _imageViewerPlugin->requestData<Clusters>(datasetName).getClusters();

			auto clustersImage = QImage(size.width(), size.height(), QImage::Format::Format_RGB32);

			const auto hueDelta = 360.0f / clusters.size();

			auto hue = 0.0f;

			for (auto& cluster : clusters) {
				for (auto id : cluster.indices) {
					const auto x = id % size.width();
					const auto y = floorf(static_cast<float>(id) / static_cast<float>(size.width()));
					clustersImage.setPixelColor(x, y, QColor::fromHsl(hue, 255, 100));
				}

				hue += hueDelta;
			}

			clustersImage.save("clustersImage.jpg");

			auto layersModel = _datasetsModel->layersModel(row);

			auto layer = new Layer(this, datasetName, datasetName, Layer::Type::MetaData, Layer::Flags::Enabled, layersModel->rowCount());

			layer->setImage(clustersImage);

			_datasetsModel->layersModel(row)->add(layer);
		}
		*/

		//qDebug() << "No. clusters: " << clusters.getClusters().size();
		/*
		if (!hits.isEmpty()) {
			const auto row = hits.first().row();

			_datasetsModel->selectionModel()->select(_datasetsModel->index(row), QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::SelectionFlag::ClearAndSelect);
			_datasetsModel->selectionModel()->setCurrentIndex(_datasetsModel->index(row), QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::SelectionFlag::ClearAndSelect);
		}
		*/
	}

	dropEvent->acceptProposedAction();
}

DatasetsModel* LayersWidget::datasetsModel()
{
	return &_imageViewerPlugin->datasetsModel();
}

LayersModel* LayersWidget::layersModel()
{
	return &_imageViewerPlugin->layersModel();
}