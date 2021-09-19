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
    _moveLayerUpAction(this, ""),
    _moveLayerDownAction(this, "")
{
    auto imageViewerPlugin = layersAction->getSettingsAction().getImageViewerPlugin();

    _removeLayerAction.setToolTip("Remove the selected layer");
    _moveLayerUpAction.setToolTip("Move the selected layer up");
    _moveLayerDownAction.setToolTip("Move the selected layer down");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _removeLayerAction.setIcon(fontAwesome.getIcon("trash-alt"));
    _moveLayerUpAction.setIcon(fontAwesome.getIcon("caret-up"));
    _moveLayerDownAction.setIcon(fontAwesome.getIcon("caret-down"));

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
    toolbarLayout->addWidget(_moveLayerUpAction.createWidget(this));
    toolbarLayout->addWidget(_moveLayerDownAction.createWidget(this));

    layout->addLayout(toolbarLayout);

    auto currentLayerWidget = layersAction->getCurrentLayerAction().createWidget(this);

    layout->addWidget(currentLayerWidget);

    setLayout(layout);

    const auto modelSelectionChanged = [this, layersAction, imageViewerPlugin, treeView, layersFilterModel, layout]() -> void {
        const auto selectedRows = treeView->selectionModel()->selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        _removeLayerAction.setEnabled(hasSelection);

        auto selectedRowIndex = hasSelection ? layersFilterModel->mapToSource(selectedRows.first()).row() : -1;

        _moveLayerUpAction.setEnabled(selectedRowIndex >= 0 ? selectedRowIndex > 0 : false);
        _moveLayerDownAction.setEnabled(selectedRowIndex >= 0 ? selectedRowIndex < layersFilterModel->rowCount() - 1 : false);

        GroupsAction::GroupActions groupActions;

        if (hasSelection) {
            auto layer = static_cast<Layer*>(layersFilterModel->mapToSource(selectedRows.first()).internalPointer());

            auto& layerAction = layer->getLayerAction();

            groupActions << &layerAction.getGeneralAction() << &layerAction.getImageAction() << &layerAction.getSelectionAction();
        }

        layersAction->getCurrentLayerAction().set(groupActions);
    };

    connect(treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this, modelSelectionChanged);

    connect(&_removeLayerAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin->getLayersModel().removeLayer(selectedRows.first());
    });

    connect(&_moveLayerUpAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin->getLayersModel().removeLayer(selectedRows.first());
    });

    connect(&_moveLayerDownAction, &TriggerAction::triggered, this, [this, imageViewerPlugin, treeView]() {
        const auto selectedRows = treeView->selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin->getLayersModel().removeLayer(selectedRows.first());
    });

    modelSelectionChanged();
}
