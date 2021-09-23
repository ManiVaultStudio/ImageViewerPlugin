#include "LayersAction.h"
#include "LayersFilterModel.h"
#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerPlugin.h"

#include "Application.h"

#include <QTreeView>
#include <QHeaderView>

using namespace hdps;
using namespace hdps::gui;

LayersAction::LayersAction(SettingsAction& settingsAction) :
    WidgetAction(reinterpret_cast<QObject*>(&settingsAction)),
    _settingsAction(settingsAction),
    _currentLayerAction(this)
{
}

LayersAction::Widget::Widget(QWidget* parent, LayersAction* layersAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, layersAction, state),
    _removeLayerAction(this, ""),
    _moveLayerToTopAction(this, ""),
    _moveLayerUpAction(this, ""),
    _moveLayerDownAction(this, ""),
    _moveLayerToBottomAction(this, "")
{
    auto imageViewerPlugin = layersAction->getSettingsAction().getImageViewerPlugin();

    _removeLayerAction.setToolTip("Remove the selected layer");
    _moveLayerToTopAction.setToolTip("Move the selected layer to the top");
    _moveLayerUpAction.setToolTip("Move the selected layer up");
    _moveLayerDownAction.setToolTip("Move the selected layer down");
    _moveLayerToBottomAction.setToolTip("Move the selected layer to the bottom");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _removeLayerAction.setIcon(fontAwesome.getIcon("trash-alt"));
    _moveLayerToTopAction.setIcon(fontAwesome.getIcon("angle-double-up"));
    _moveLayerUpAction.setIcon(fontAwesome.getIcon("angle-up"));
    _moveLayerDownAction.setIcon(fontAwesome.getIcon("angle-down"));
    _moveLayerToBottomAction.setIcon(fontAwesome.getIcon("angle-double-down"));

    auto layout             = new QVBoxLayout();
    auto treeView           = new QTreeView();
    auto layersFilterModel  = new LayersFilterModel(this);

    layersFilterModel->setSourceModel(&imageViewerPlugin->getLayersModel());

    treeView->setFixedHeight(200);
    treeView->setModel(layersFilterModel);
    treeView->setRootIsDecorated(false);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setSortingEnabled(false);

    // Configure header view
    auto header = treeView->header();

    header->setStretchLastSection(true);

    const auto minimumSectionSize = 20;

    header->setMinimumSectionSize(minimumSectionSize);
    header->hideSection(LayersModel::Color);

    header->resizeSection(LayersModel::Visible, minimumSectionSize);
    header->resizeSection(LayersModel::Color, minimumSectionSize);
    header->resizeSection(LayersModel::ImageWidth, 50);
    header->resizeSection(LayersModel::ImageHeight, 50);
    header->resizeSection(LayersModel::Scale, 50);
    header->resizeSection(LayersModel::Opacity, 50);

    header->setSectionResizeMode(LayersModel::Visible, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Color, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Name, QHeaderView::Interactive);
    header->setSectionResizeMode(LayersModel::ImageWidth, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::ImageHeight, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Scale, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Opacity, QHeaderView::Fixed);

    layout->setMargin(0);
    layout->addWidget(treeView);

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->setSpacing(3);

    toolbarLayout->addWidget(_removeLayerAction.createWidget(this));
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_moveLayerToTopAction.createWidget(this));
    toolbarLayout->addWidget(_moveLayerUpAction.createWidget(this));
    toolbarLayout->addWidget(_moveLayerDownAction.createWidget(this));
    toolbarLayout->addWidget(_moveLayerToBottomAction.createWidget(this));

    layout->addLayout(toolbarLayout);

    auto currentLayerWidget = layersAction->getCurrentLayerAction().createWidget(this);

    layout->addWidget(currentLayerWidget);

    setLayout(layout);

    const auto modelSelectionChanged = [this, layersAction, imageViewerPlugin, treeView, layersFilterModel, layout]() -> void {
        const auto selectedRows = treeView->selectionModel()->selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        GroupsAction::GroupActions groupActions;

        if (hasSelection) {
            auto layer = static_cast<Layer*>(layersFilterModel->mapToSource(selectedRows.first()).internalPointer());

            auto& layerAction = layer->getLayerAction();

            groupActions << &layerAction.getGeneralAction() << &layerAction.getImageAction() << &layerAction.getSelectionAction();
        }

        layersAction->getCurrentLayerAction().set(groupActions);

        // Enable the pixel selection tool when there is a selection and disable otherwise
        imageViewerPlugin->getImageViewerWidget()->getPixelSelectionTool().setEnabled(hasSelection);
    };

    const auto updateButtons = [this, treeView, layersFilterModel, layout]() -> void {
        const auto selectedRows = treeView->selectionModel()->selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        _removeLayerAction.setEnabled(hasSelection);

        auto selectedRowIndex = hasSelection ? layersFilterModel->mapToSource(selectedRows.first()).row() : -1;

        _moveLayerToTopAction.setEnabled(hasSelection && selectedRowIndex > 0);
        _moveLayerUpAction.setEnabled(selectedRowIndex > 0 ? selectedRowIndex > 0 : false);
        _moveLayerDownAction.setEnabled(selectedRowIndex >= 0 ? selectedRowIndex < layersFilterModel->rowCount() - 1 : false);
        _moveLayerToBottomAction.setEnabled(hasSelection && selectedRowIndex < layersFilterModel->rowCount() - 1);
    };

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, modelSelectionChanged);
    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsInserted, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsRemoved, updateButtons);
    connect(treeView->model(), &QAbstractListModel::layoutChanged, updateButtons);

    connect(&_removeLayerAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin->getLayersModel().removeLayer(selectedRows.first());
    });

    connect(&_moveLayerToTopAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin->getLayersModel().moveLayer(selectedRows.first(), -1000);

    });
    connect(&_moveLayerUpAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin->getLayersModel().moveLayer(selectedRows.first(), -1);
    });

    connect(&_moveLayerDownAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin->getLayersModel().moveLayer(selectedRows.first(), 1);
    });

    connect(&_moveLayerToBottomAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin->getLayersModel().moveLayer(selectedRows.first(), 1000);
    });

    updateButtons();
    modelSelectionChanged();
}
