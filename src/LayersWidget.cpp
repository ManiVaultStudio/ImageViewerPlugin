//#include "LayersWidget.h"
//#include "ImageViewerPlugin.h"
//#include "ViewerWidget.h"
//#include "LayersModel.h"
//#include "SelectionLayer.h"
//#include "Layer.h"
//#include "GroupLayer.h"
//#include "PointsLayer.h"
//#include "SelectionLayer.h"
//#include "LayerWidget.h"
//
//#include "ui_LayersWidget.h"
//
//#include "PointData.h"
//#include "Application.h"
//
//#include <QItemSelectionModel>
//#include <QDebug>
//#include <QDragEnterEvent>
//#include <QMimeData>
//#include <QScrollArea>
//
//LayersWidget::LayersWidget(QWidget* parent) :
//    QWidget(parent),
//    _imageViewerPlugin(nullptr),
//    _ui{ std::make_unique<Ui::LayersWidget>() },
//    _scrollArea(new QScrollArea(this)),
//    _layerWidget(new LayerWidget(this))
//{
//    _ui->setupUi(this);
//}
//
//LayersWidget::~LayersWidget() = default;
//
//void LayersWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
//{
//    _imageViewerPlugin = imageViewerPlugin;
//
//    _layerWidget->initialize(_imageViewerPlugin);
//
//    layout()->addWidget(_scrollArea);
//
//    _scrollArea->setWidget(_layerWidget);
//    _scrollArea->setWidgetResizable(true);
//    _scrollArea->setStyleSheet("QScrollArea { border: none; }");
//
//    _ui->layersTreeView->setModel(&_imageViewerPlugin->getLayersModel());
//    _ui->layersTreeView->setSelectionModel(&getLayersSelectionModel());
//    _ui->layersTreeView->setDragDropOverwriteMode(false);
//    _ui->layersTreeView->setHeaderHidden(false);
//
//    QFont font = QFont("Font Awesome 5 Free Solid", 9);
//
//    _ui->layerRemovePushButton->setFont(font);
//    _ui->layerMoveUpPushButton->setFont(font);
//    _ui->layerMoveDownPushButton->setFont(font);
//
//    _ui->layerRemovePushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("trash-alt"));
//    _ui->layerMoveUpPushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("caret-up"));
//    _ui->layerMoveDownPushButton->setText(hdps::Application::getIconFont("FontAwesome").getIconCharacter("caret-down"));
//
//    QObject::connect(_ui->layerMoveUpPushButton, &QPushButton::clicked, [this]() {
//        const auto selectedRows = getLayersSelectionModel().selectedRows();
//
//        if (selectedRows.size() == 1) {
//            const auto firstRow = selectedRows.first();
//            const auto parent = firstRow.parent();
//
//            getLayersModel().moveLayer(parent, firstRow.row(), parent, firstRow.row() - 1);
//        }
//    });
//
//    QObject::connect(_ui->layerMoveDownPushButton, &QPushButton::clicked, [this]() {
//        const auto selectedRows = getLayersSelectionModel().selectedRows();
//
//        if (selectedRows.size() == 1) {
//            const auto firstRow = selectedRows.first();
//            const auto parent = firstRow.parent();
//
//            getLayersModel().moveLayer(parent, firstRow.row() + 1, parent, firstRow.row());
//        }
//    });
//
//    QObject::connect(_ui->layerRemovePushButton, &QPushButton::clicked, [this]() {
//        
//        getLayersModel().removeLayers(getLayersSelectionModel().selectedRows());
//
//        if (getLayersModel().rowCount() >= 1)
//            getLayersModel().selectRow(0);
//    });
//
//    _ui->layersGroupBox->setEnabled(true);
//
//    auto headerView = _ui->layersTreeView->header();
//
//    headerView->setSectionResizeMode(QHeaderView::ResizeToContents);
//    headerView->hide();
//
//    for (int column = ult(Layer::Column::Start); column <= ult(Layer::Column::End); column++)
//        headerView->hideSection(column);
//
//    headerView->showSection(ult(Layer::Column::Name));
//    headerView->showSection(ult(Layer::Column::Type));
//    headerView->showSection(ult(Layer::Column::Opacity));
//    headerView->showSection(ult(Layer::Column::ImageWidth));
//    headerView->showSection(ult(Layer::Column::ImageHeight));
//    headerView->showSection(ult(Layer::Column::Scale));
//
//    headerView->setSectionResizeMode(ult(Layer::Column::Name), QHeaderView::Interactive);
//
//    auto updateButtons = [this]() {
//        const auto selectedRows = getLayersSelectionModel().selectedRows();
//        const auto noSelectedRows = selectedRows.size();
//
//        _ui->layerRemovePushButton->setEnabled(false);
//        _ui->layerMoveUpPushButton->setEnabled(false);
//        _ui->layerMoveDownPushButton->setEnabled(false);
//
//        _ui->layerRemovePushButton->setToolTip("");
//        _ui->layerMoveUpPushButton->setToolTip("");
//        _ui->layerMoveDownPushButton->setToolTip("");
//
//        if (noSelectedRows == 1) {
//            const auto firstRow = selectedRows.first();
//            const auto row = firstRow.row();
//            const auto name = firstRow.siblingAtColumn(ult(Layer::Column::Name)).data(Qt::EditRole).toString();
//
//            const auto mayMoveUp = getLayersModel().mayMoveLayer(firstRow, -1);
//
//            _ui->layerMoveUpPushButton->setEnabled(mayMoveUp);
//            _ui->layerMoveUpPushButton->setToolTip(mayMoveUp ? QString("Move %1 up one level").arg(name) : "");
//
//            const auto mayMoveDown = getLayersModel().mayMoveLayer(firstRow, 1);
//
//            _ui->layerMoveDownPushButton->setEnabled(mayMoveDown);
//            _ui->layerMoveDownPushButton->setToolTip(mayMoveDown ? QString("Move %1 down one level").arg(name) : "");
//        }
//
//        _ui->layerRemovePushButton->setEnabled(noSelectedRows > 0);
//    };
//
//    QObject::connect(&getLayersModel(), &QAbstractItemModel::rowsMoved, [this, updateButtons](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row) {
//        updateButtons();
//    });
//
//    QObject::connect(&getLayersModel(), &QAbstractItemModel::rowsInserted, [this, updateButtons](const QModelIndex &parent, int first, int last) {
//        _ui->layersTreeView->header()->setVisible(getLayersModel().rowCount() > 0);
//    });
//
//    QObject::connect(&getLayersModel(), &QAbstractItemModel::rowsRemoved, [this, updateButtons](const QModelIndex &parent, int first, int last) {
//        _ui->layersTreeView->header()->setVisible(getLayersModel().rowCount() > 0);
//    });
//
//    QObject::connect(&getLayersSelectionModel(), &QItemSelectionModel::selectionChanged, [this, updateButtons](const QItemSelection &selected, const QItemSelection &deselected) {
//        updateButtons();
//
//        _imageViewerPlugin->getViewerWidget()->update();
//    });
//}
//
//LayersModel& LayersWidget::getLayersModel()
//{
//    return _imageViewerPlugin->getLayersModel();
//}
//
//QItemSelectionModel& LayersWidget::getLayersSelectionModel()
//{
//    return getLayersModel().getSelectionModel();
//}