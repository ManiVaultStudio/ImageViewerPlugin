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

	_ui->layerWidget->initialize(layersModel());
	_ui->pointsLayerWidget->initialize(layersModel());
	_ui->imagesLayerWidget->initialize(layersModel());
	_ui->clustersLayerWidget->initialize(layersModel());

	_ui->datasetsTreeView->setModel(&_imageViewerPlugin->datasetsModel());

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
	_ui->layerWidget->updateData(topLeft, bottomRight, roles);
	_ui->pointsLayerWidget->updateData(topLeft, bottomRight, roles);
	_ui->imagesLayerWidget->updateData(topLeft, bottomRight, roles);
	_ui->clustersLayerWidget->updateData(topLeft, bottomRight, roles);
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