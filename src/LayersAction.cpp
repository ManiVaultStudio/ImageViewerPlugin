#include "LayersAction.h"
#include "LayersFilterModel.h"
#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerPlugin.h"

#include "Application.h"

#include <QTreeView>

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

    treeView->setFixedHeight(400);
    treeView->setModel(layersFilterModel);
    treeView->setRootIsDecorated(false);

    layout->setMargin(0);
    layout->addWidget(treeView);

    auto toolbarLayout = new QHBoxLayout();

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
    
    connect(treeView->model(), &QAbstractListModel::dataChanged, this, [treeView](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) {
        treeView->update();
    });

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
