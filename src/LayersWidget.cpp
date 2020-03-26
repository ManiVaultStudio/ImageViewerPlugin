#include "LayersWidget.h"
#include "ImageViewerPlugin.h"
#include "LayersModel.h"
#include "SelectionItem.h"
#include "LayerItem.h"
#include "RootItem.h"

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

	
	_ui->layerWidget->initialize(layersModel());
	_ui->datasetsTreeView->setModel(&_imageViewerPlugin->datasetsModel());

	_ui->layersTreeView->setModel(layersModel());
	_ui->layersTreeView->setSelectionModel(layersModel()->selectionModel());

	//layersModel()->setSelectionModel(_ui->datasetsTreeView->selectionModel());

	QFont font = QFont("Font Awesome 5 Free Solid", 9);

	_ui->layerRemovePushButton->setFont(font);
	_ui->layerMoveUpPushButton->setFont(font);
	_ui->layerMoveDownPushButton->setFont(font);

	_ui->layerRemovePushButton->setText(u8"\uf2ed");
	_ui->layerMoveUpPushButton->setText(u8"\uf0d8");
	_ui->layerMoveDownPushButton->setText(u8"\uf0d7");

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		layersModel()->moveUp(layersModel()->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		layersModel()->moveDown(layersModel()->selectionModel()->currentIndex().row());
	});

	QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
		layersModel()->removeRows(layersModel()->selectionModel()->selectedRows());
	});

	_ui->layersGroupBox->setEnabled(true);
	//_ui->layersTreeView->setModel(layersModel());

	auto headerView = _ui->layersTreeView->header();
/*
	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	for (int column = to_underlying(LayerItem::Column::Enabled); column < to_underlying(LayerItem::Column::DisplayRange); column++)
		headerView->hideSection(column);

	headerView->showSection(to_underlying(LayerItem::Column::Type));
	headerView->showSection(to_underlying(LayerItem::Column::Name));
	headerView->showSection(to_underlying(LayerItem::Column::Opacity));

	headerView->setSectionResizeMode(to_underlying(LayerItem::Column::Name), QHeaderView::Interactive);

	
	auto updateButtons = [this]() {
		const auto selectedRows = layersModel()->selectionModel()->selectedRows();
		const auto noSelectedRows = selectedRows.size();

		_ui->layerRemovePushButton->setEnabled(false);
		_ui->layerMoveUpPushButton->setEnabled(false);
		_ui->layerMoveDownPushButton->setEnabled(false);

		_ui->layerRemovePushButton->setToolTip("");
		_ui->layerMoveUpPushButton->setToolTip("");
		_ui->layerMoveDownPushButton->setToolTip("");

		if (noSelectedRows == 1) {
			const auto row = selectedRows.at(0).row();
			const auto name = layersModel()->data(row, to_underlying(LayerItem::Column::Name), Qt::EditRole).toString();
			const auto mayRemove = layersModel()->data(row, to_underlying(LayerItem::Column::Removable), Qt::EditRole).toBool();

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
				if (layersModel()->data(index.row(), to_underlying(LayerItem::Column::Removable), Qt::EditRole).toBool()) {
					mayRemove = true;
					names << layersModel()->data(index.row(), to_underlying(LayerItem::Column::Name), Qt::EditRole).toString();
				}
			}

			if (mayRemove) {
				_ui->layerRemovePushButton->setEnabled(true);
				_ui->layerRemovePushButton->setToolTip(QString("Remove %1").arg(names.join(", ")));
			}
		}
	};

	QObject::connect(layersModel()->selectionModel(), &QItemSelectionModel::selectionChanged, [this, updateButtons](const QItemSelection &selected, const QItemSelection &deselected) {
		updateButtons();

		const auto selectedRows = layersModel()->selectionModel()->selectedRows();

		if (selectedRows.isEmpty())
			_ui->layerWidget->onDataChanged(layersModel()->index(0, 0), layersModel()->index(0, layersModel()->columnCount() - 1));
		else
			_ui->layerWidget->onDataChanged(layersModel()->index(selectedRows.first().row(), 0), layersModel()->index(selectedRows.first().row(), layersModel()->columnCount() - 1));
	});

	// Handle model rows reorganization
	QObject::connect(layersModel(), &QAbstractListModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateButtons();
	});
	*/

	//layersModel()->selectionModel()->setCurrentIndex(layersModel()->index(0), QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items = dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetName = items.at(0);
	const auto datasetType = items.at(1);
	
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
	const auto layerFlags				= static_cast<int>(LayerItem::Flag::Enabled) | static_cast<int>(LayerItem::Flag::Removable);

	if (datasetType == "Points") {
		const auto points = _imageViewerPlugin->requestData<Points>(datasetName);

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Points);
		
		layersModel()->addLayer(dataset, LayerItem::Type::Points, datasetName, datasetName, layerFlags);

		if (createSelectionLayer)
			layersModel()->addLayer(dataset, LayerItem::Type::Selection, selectionName, selectionName, layerFlags);

		dataset->init();
	}

	if (datasetType == "Images") {
		const auto imagesName		= datasetName;
		const auto selectionName	= QString("%1_selection").arg(datasetName);

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Images);

		/*
		layersModel()->addLayer(new ImagesLayer(dataset, imagesName, imagesName, layerFlags));

		if (createSelectionLayer)
			layersModel()->addLayer(new SelectionLayer(dataset, selectionName, selectionName, layerFlags));
		*/

		dataset->init();
	}

	if (datasetType == "Clusters") {

		auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Clusters);
	}

	dropEvent->acceptProposedAction();
	/**/
}

DatasetsModel* LayersWidget::datasetsModel()
{
	return &_imageViewerPlugin->datasetsModel();
}

LayersModel* LayersWidget::layersModel()
{
	return &_imageViewerPlugin->layersModel();
}