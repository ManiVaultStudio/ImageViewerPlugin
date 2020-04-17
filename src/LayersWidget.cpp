#include "LayersWidget.h"
#include "ImageViewerPlugin.h"
#include "LayersModel.h"
#include "SelectionLayer.h"
#include "LayerNode.h"
#include "GroupLayer.h"
#include "PointsLayer.h"
#include "ImagesLayer.h"
#include "ClustersLayer.h"
#include "SelectionLayer.h"

#include "ui_LayersWidget.h"

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

	_ui->layerWidget->initialize(_imageViewerPlugin);

	_ui->layersTreeView->setModel(&_imageViewerPlugin->layersModel());
	_ui->layersTreeView->setSelectionModel(&layersSelectionModel());
	_ui->layersTreeView->setDragDropOverwriteMode(false);
	_ui->layersTreeView->setHeaderHidden(false);

	QFont font = QFont("Font Awesome 5 Free Solid", 9);

	_ui->layerRemovePushButton->setFont(font);
	_ui->layerMoveUpPushButton->setFont(font);
	_ui->layerMoveDownPushButton->setFont(font);

	_ui->layerRemovePushButton->setText(u8"\uf2ed");
	_ui->layerMoveUpPushButton->setText(u8"\uf0d8");
	_ui->layerMoveDownPushButton->setText(u8"\uf0d7");

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();

		if (selectedRows.size() == 1) {
			const auto firstRow = selectedRows.first();
			const auto parent	= firstRow.parent();

			layersModel().moveLayer(parent, firstRow.row(), parent, firstRow.row() - 1);
		}
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();

		if (selectedRows.size() == 1) {
			const auto firstRow = selectedRows.first();
			const auto parent	= firstRow.parent();
			
			layersModel().moveLayer(parent, firstRow.row() + 1, parent, firstRow.row());
		}
	});

	QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();

		if (selectedRows.size() == 1)
			layersModel().removeLayer(selectedRows.first());
	});

	_ui->layersGroupBox->setEnabled(true);
	
	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);

	//for (int column = ult(LayerNode::Column::Start); column < ult(LayerNode::Column::End); column++)
	//	headerView->hideSection(column);

	headerView->showSection(ult(LayerNode::Column::Name));
	//headerView->showSection(ult(Layer::Column::Enabled));
	//headerView->showSection(ult(Layer::Column::Type));
	headerView->showSection(ult(LayerNode::Column::Opacity));
	//headerView->showSection(ult(Layer::Column::Enabled));

	headerView->setSectionResizeMode(ult(LayerNode::Column::Name), QHeaderView::Interactive);

	auto updateButtons = [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();
		const auto noSelectedRows = selectedRows.size();

		_ui->layerRemovePushButton->setEnabled(false);
		_ui->layerMoveUpPushButton->setEnabled(false);
		_ui->layerMoveDownPushButton->setEnabled(false);

		_ui->layerRemovePushButton->setToolTip("");
		_ui->layerMoveUpPushButton->setToolTip("");
		_ui->layerMoveDownPushButton->setToolTip("");
		
		if (noSelectedRows == 1) {
			const auto firstRow		= selectedRows.first();
			const auto row			= firstRow.row();
			const auto name			= firstRow.siblingAtColumn(ult(LayerNode::Column::Name)).data(Qt::EditRole).toString();
			
			const auto mayMoveUp = layersModel().mayMoveLayer(firstRow, -1);

			_ui->layerMoveUpPushButton->setEnabled(mayMoveUp);
			_ui->layerMoveUpPushButton->setToolTip(mayMoveUp ? QString("Move %1 up one level").arg(name) : "");

			const auto mayMoveDown = layersModel().mayMoveLayer(firstRow, 1);

			_ui->layerMoveDownPushButton->setEnabled(mayMoveDown);
			_ui->layerMoveDownPushButton->setToolTip(mayMoveDown ? QString("Move %1 down one level").arg(name) : "");
		}
		
		_ui->layerRemovePushButton->setEnabled(noSelectedRows == 1);
	};
	
	QObject::connect(&layersModel(), &QAbstractListModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateButtons();
	});

	QObject::connect(&layersSelectionModel(), &QItemSelectionModel::selectionChanged, [this, updateButtons](const QItemSelection &selected, const QItemSelection &deselected) {
		updateButtons();
	});
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items		= dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetName	= items.at(0);
	const auto datasetType	= items.at(1);
	
	if (datasetType == "Points") {
		auto imagesSet = _imageViewerPlugin->sourceImagesSetFromPointsSet(datasetName);

		if (imagesSet == nullptr)
			return;

		if (imagesSet->points()->getNumDimensions() >= 1)
			dragEnterEvent->acceptProposedAction();
	}

	if (datasetType == "Images")
		dragEnterEvent->acceptProposedAction();

	if (datasetType == "Clusters")
		dragEnterEvent->acceptProposedAction();
}

void LayersWidget::dropEvent(QDropEvent* dropEvent)
{
	const auto items			= dropEvent->mimeData()->text().split("\n");
	const auto datasetName		= items.at(0);
	const auto datasetType		= items.at(1);
	const auto selectionName	= QString("%1_selection").arg(datasetName);
	//const auto createSelectionLayer		= layersModel().findLayerById(selectionName) == nullptr;
	const auto layerFlags		= ult(LayerNode::Flag::Enabled) | ult(LayerNode::Flag::Renamable);

	const auto rootIndex = layersModel().index(0, 0);

	if (datasetType == "Points") {
		const auto points = _imageViewerPlugin->requestData<Points>(datasetName);

		auto sourcePoints = hdps::DataSet::getSourceData(points);

		//_imageViewerPlugin->core()->getDataManager()
		layersModel().insertLayer(0, new PointsLayer(datasetName, datasetName, datasetName, layerFlags));
		
		/*
		if (createSelectionLayer)
			layersModel().addLayer(new SelectionLayer(dataset, selectionName, selectionName, layerFlags));
		*/
	}

	if (datasetType == "Images") {
		const auto imagesName		= datasetName;
		const auto selectionName	= QString("%1_selection").arg(datasetName);

		//auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Images);

		layersModel().insertLayer(0, new ImagesLayer(datasetName, imagesName, imagesName, layerFlags));

		/*
		if (createSelectionLayer)
			layersModel().addLayer(new SelectionLayer(dataset, selectionName, selectionName, layerFlags));
		*/
	}

	if (datasetType == "Clusters") {

		//auto dataset = datasetsModel()->addDataset(datasetName, Dataset::Type::Clusters);
	}

	dropEvent->acceptProposedAction();
}

LayersModel& LayersWidget::layersModel()
{
	return _imageViewerPlugin->layersModel();
}

QItemSelectionModel& LayersWidget::layersSelectionModel()
{
	return layersModel().selectionModel();
}