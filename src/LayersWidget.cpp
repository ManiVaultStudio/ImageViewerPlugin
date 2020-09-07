#include "LayersWidget.h"
#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "LayersModel.h"
#include "SelectionLayer.h"
#include "Layer.h"
#include "GroupLayer.h"
#include "PointsLayer.h"
#include "SelectionLayer.h"

#include "ui_LayersWidget.h"

#include "PointData.h"
#include "Application.h"

#include <QItemSelectionModel>
#include <QDebug>
#include <QDragEnterEvent>
#include <QMimeData>

LayersWidget::LayersWidget(QWidget* parent) :
	QWidget(parent),
	_imageViewerPlugin(nullptr),
	_ui{ std::make_unique<Ui::LayersWidget>() }
{
	setAcceptDrops(true);

	_ui->setupUi(this);
}

LayersWidget::~LayersWidget() = default;

void LayersWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;

	_ui->layerWidget->initialize(_imageViewerPlugin);

	_ui->layersTreeView->setModel(&_imageViewerPlugin->layersModel());
	_ui->layersTreeView->setSelectionModel(&layersSelectionModel());
	_ui->layersTreeView->setDragDropOverwriteMode(false);
	_ui->layersTreeView->setHeaderHidden(false);

	QFont font = QFont("Font Awesome 5 Free Solid", 9);

	_ui->layerRemovePushButton->setFont(font);
	_ui->layerMoveUpPushButton->setFont(font);
	_ui->layerMoveDownPushButton->setFont(font);

	_ui->layerRemovePushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("trash-alt"));
	_ui->layerMoveUpPushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("caret-up"));
	_ui->layerMoveDownPushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("caret-down"));

	QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();

		if (selectedRows.size() == 1) {
			const auto firstRow = selectedRows.first();
			const auto parent = firstRow.parent();

			layersModel().moveLayer(parent, firstRow.row(), parent, firstRow.row() - 1);
		}
	});

	QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
		const auto selectedRows = layersSelectionModel().selectedRows();

		if (selectedRows.size() == 1) {
			const auto firstRow = selectedRows.first();
			const auto parent = firstRow.parent();

			layersModel().moveLayer(parent, firstRow.row() + 1, parent, firstRow.row());
		}
	});

	QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
		for (auto selectedRow : layersSelectionModel().selectedRows()) {
			layersModel().removeLayer(selectedRow);
			layersModel().selectRow(0);
		}
	});

	_ui->layersGroupBox->setEnabled(true);

	auto headerView = _ui->layersTreeView->header();

	headerView->setSectionResizeMode(QHeaderView::ResizeToContents);
	headerView->hide();

	for (int column = ult(Layer::Column::Start); column <= ult(Layer::Column::End); column++)
		headerView->hideSection(column);

	headerView->showSection(ult(Layer::Column::Name));
	headerView->showSection(ult(Layer::Column::Type));
	headerView->showSection(ult(Layer::Column::Opacity));
	headerView->showSection(ult(Layer::Column::ImageWidth));
	headerView->showSection(ult(Layer::Column::ImageHeight));
	headerView->showSection(ult(Layer::Column::Scale));

	headerView->setSectionResizeMode(ult(Layer::Column::Name), QHeaderView::Interactive);

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
			const auto firstRow = selectedRows.first();
			const auto row = firstRow.row();
			const auto name = firstRow.siblingAtColumn(ult(Layer::Column::Name)).data(Qt::EditRole).toString();

			const auto mayMoveUp = layersModel().mayMoveLayer(firstRow, -1);

			_ui->layerMoveUpPushButton->setEnabled(mayMoveUp);
			_ui->layerMoveUpPushButton->setToolTip(mayMoveUp ? QString("Move %1 up one level").arg(name) : "");

			const auto mayMoveDown = layersModel().mayMoveLayer(firstRow, 1);

			_ui->layerMoveDownPushButton->setEnabled(mayMoveDown);
			_ui->layerMoveDownPushButton->setToolTip(mayMoveDown ? QString("Move %1 down one level").arg(name) : "");
		}

		_ui->layerRemovePushButton->setEnabled(noSelectedRows > 0);
	};

	QObject::connect(&layersModel(), &QAbstractItemModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
		updateButtons();
	});

	QObject::connect(&layersModel(), &QAbstractItemModel::rowsInserted, [this, updateButtons](const QModelIndex &parent, int first, int last) {
		_ui->layersTreeView->header()->setVisible(layersModel().rowCount() > 0);
	});

	QObject::connect(&layersModel(), &QAbstractItemModel::rowsRemoved, [this, updateButtons](const QModelIndex &parent, int first, int last) {
		_ui->layersTreeView->header()->setVisible(layersModel().rowCount() > 0);
	});

	QObject::connect(&layersSelectionModel(), &QItemSelectionModel::selectionChanged, [this, updateButtons](const QItemSelection &selected, const QItemSelection &deselected) {
		updateButtons();

		_imageViewerPlugin->viewerWidget()->update();
	});
}

void LayersWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
	const auto items		= dragEnterEvent->mimeData()->text().split("\n");
	const auto datasetName	= items.at(0);
	const auto datasetType	= items.at(1);
	
	if (datasetType == "Points") {
		auto pointsDataset = _imageViewerPlugin->requestData<Points>(datasetName);
		
		if (pointsDataset.isDerivedData()) {
			auto sourcePointsDataset = hdps::DataSet::getSourceData<Points>(pointsDataset);

			if (sourcePointsDataset.getProperty("Type", "").toString() == "Images")
				dragEnterEvent->acceptProposedAction();
		}
		else {
			if (pointsDataset.getProperty("Type", "").toString() == "Images")
				dragEnterEvent->acceptProposedAction();
		}
	}

	if (datasetType == "Clusters")
		dragEnterEvent->acceptProposedAction();
}

void LayersWidget::dropEvent(QDropEvent* dropEvent)
{
	const auto items					= dropEvent->mimeData()->text().split("\n");
	const auto datasetName				= items.at(0);
	const auto datasetType				= items.at(1);
	const auto selectionName			= QString("%1_selection").arg(datasetName);
	const auto selectionLayerIndices	= layersModel().match(layersModel().index(0, ult(Layer::Column::ID)), Qt::DisplayRole, selectionName, -1, Qt::MatchExactly);
	const auto createSelectionLayer		= selectionLayerIndices.isEmpty();
	const auto layerFlags				= ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable);

	auto largestImageSize = QSize();

	for (auto imageLayerIndex : layersModel().match(layersModel().index(0, ult(Layer::Column::Type)), Qt::EditRole, ult(Layer::Type::Points), -1, Qt::MatchExactly | Qt::MatchRecursive)) {
		const auto imageSize = layersModel().data(imageLayerIndex.siblingAtColumn(ult(Layer::Column::ImageSize)), Qt::EditRole).toSize();

		if (imageSize.width() > largestImageSize.width() && imageSize.height() > largestImageSize.height())
			largestImageSize = imageSize;
	}

	if (datasetType == "Points") {
		auto pointsLayer = new PointsLayer(datasetName, datasetName, datasetName, layerFlags);

		if (largestImageSize.isValid())
			pointsLayer->matchScaling(largestImageSize);

		qDebug() << pointsLayer->imageCollectionType();

		if (pointsLayer->imageCollectionType() == ult(ImageData::Type::Stack) && createSelectionLayer) {
			auto selectionLayer = new SelectionLayer(datasetName, selectionName, selectionName, layerFlags);

			selectionLayer->setOpacity(0.8f);

			if (largestImageSize.isValid())
				selectionLayer->matchScaling(largestImageSize);

			layersModel().insertLayer(0, pointsLayer);
			layersModel().insertLayer(0, selectionLayer);
			layersModel().selectRow(1);
		}
		else {
			const auto row = selectionLayerIndices.isEmpty() ? 0 : selectionLayerIndices.first().row() + 1;

			layersModel().insertLayer(row, pointsLayer);
			layersModel().selectRow(row);
		}
	}

	if (datasetType == "Clusters") {
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