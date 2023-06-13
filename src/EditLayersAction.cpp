#include "EditLayersAction.h"
#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsAction.h"

#include <Application.h>

#include <widgets/Divider.h>

#include <QTreeView>
#include <QHeaderView>

using namespace hdps;
using namespace hdps::gui;

QRandomGenerator EditLayersAction::rng;

EditLayersAction::EditLayersAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _settingsAction(*static_cast<SettingsAction*>(parent)),
    _removeLayerAction(this, "Remove"),
    _duplicateLayerAction(this, "Duplicate"),
    _moveLayerToTopAction(this, "Move To Top"),
    _moveLayerUpAction(this, "Move Up"),
    _moveLayerDownAction(this, "Move Down"),
    _moveLayerToBottomAction(this, "Move To Bottom"),
    _operationsAction(this, "Operations")
{
    setConnectionPermissionsToForceNone(true);

    _operationsAction.setDefaultWidgetFlags(GroupAction::Horizontal);

    _operationsAction.addAction(&_removeLayerAction, TriggerAction::Icon);
    _operationsAction.addAction(&_moveLayerToTopAction, TriggerAction::Icon);
    _operationsAction.addAction(&_moveLayerUpAction, TriggerAction::Icon);
    _operationsAction.addAction(&_moveLayerDownAction, TriggerAction::Icon);
    _operationsAction.addAction(&_moveLayerToBottomAction, TriggerAction::Icon);

    _removeLayerAction.setToolTip("Remove the selected layer");
    _duplicateLayerAction.setToolTip("Duplicate the selected layer");
    _moveLayerToTopAction.setToolTip("Move the selected layer to the top");
    _moveLayerUpAction.setToolTip("Move the selected layer up");
    _moveLayerDownAction.setToolTip("Move the selected layer down");
    _moveLayerToBottomAction.setToolTip("Move the selected layer to the bottom");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _removeLayerAction.setIcon(fontAwesome.getIcon("trash-alt"));
    _duplicateLayerAction.setIcon(fontAwesome.getIcon("clone"));
    _moveLayerToTopAction.setIcon(fontAwesome.getIcon("angle-double-up"));
    _moveLayerUpAction.setIcon(fontAwesome.getIcon("angle-up"));
    _moveLayerDownAction.setIcon(fontAwesome.getIcon("angle-down"));
    _moveLayerToBottomAction.setIcon(fontAwesome.getIcon("angle-double-down"));

    connect(&_settingsAction.getImageViewerPlugin().getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [](const QItemSelection& selected, const QItemSelection& deselected) {
        if (!deselected.indexes().isEmpty()) {
            auto layer = static_cast<Layer*>(deselected.indexes().first().internalPointer());

            layer->deactivate();
        }

        if (!selected.indexes().isEmpty()) {
            auto layer = static_cast<Layer*>(selected.indexes().first().internalPointer());

            layer->activate();
        }
    });
}

QColor EditLayersAction::getRandomLayerColor()
{
    const auto randomHue        = rng.bounded(360);
    const auto randomSaturation = rng.bounded(150, 255);
    const auto randomLightness  = rng.bounded(150, 220);

    return QColor::fromHsl(randomHue, randomSaturation, randomLightness);
}

EditLayersAction::Widget::Widget(QWidget* parent, EditLayersAction* editLayersAction) :
    WidgetActionWidget(parent, editLayersAction)
{
    auto& imageViewerPlugin = editLayersAction->getSettingsAction().getImageViewerPlugin();

    auto layout     = new QVBoxLayout();
    auto treeView   = new QTreeView();

    treeView->setFixedHeight(200);
    treeView->setModel(&imageViewerPlugin.getModel());
    treeView->setRootIsDecorated(false);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setSelectionModel(&imageViewerPlugin.getSelectionModel());
    treeView->setSortingEnabled(false);

    // Configure header view
    auto header = treeView->header();

    header->setStretchLastSection(true);

    const auto minimumSectionSize = 20;

    header->setMinimumSectionSize(minimumSectionSize);
    //header->hideSection(LayersModel::Color);

    header->resizeSection(LayersModel::Visible, minimumSectionSize);
    header->resizeSection(LayersModel::Color, minimumSectionSize);
    header->resizeSection(LayersModel::ImageWidth, 50);
    header->resizeSection(LayersModel::ImageHeight, 50);
    header->resizeSection(LayersModel::Scale, 60);
    header->resizeSection(LayersModel::Opacity, 50);

    header->setSectionResizeMode(LayersModel::Visible, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Color, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Name, QHeaderView::Interactive);
    header->setSectionResizeMode(LayersModel::ImageWidth, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::ImageHeight, QHeaderView::Fixed);
    header->setSectionResizeMode(LayersModel::Scale, QHeaderView::Interactive);
    header->setSectionResizeMode(LayersModel::Opacity, QHeaderView::Fixed);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(treeView);
    layout->addWidget(editLayersAction->getOperationsAction().createWidget(this));

    setLayout(layout);

    const auto updateButtons = [this, &imageViewerPlugin, treeView, editLayersAction]() -> void {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        editLayersAction->getRemoveLayerAction().setEnabled(hasSelection);
        editLayersAction->getDuplicateLayerAction().setEnabled(hasSelection);

        auto selectedRowIndex = hasSelection ? selectedRows.first().row() : -1;

        editLayersAction->getMoveLayerToTopAction().setEnabled(hasSelection && selectedRowIndex > 0);
        editLayersAction->getMoveLayerUpAction().setEnabled(selectedRowIndex > 0 ? selectedRowIndex > 0 : false);
        editLayersAction->getMoveLayerDownAction().setEnabled(selectedRowIndex >= 0 ? selectedRowIndex < treeView->model()->rowCount() - 1 : false);
        editLayersAction->getMoveLayerToBottomAction().setEnabled(hasSelection && selectedRowIndex < treeView->model()->rowCount() - 1);

        imageViewerPlugin.getImageViewerWidget().update();
    };

    connect(&imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsRemoved, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsMoved, updateButtons);
    connect(treeView->model(), &QAbstractListModel::layoutChanged, updateButtons);

    const auto onRowsInserted = [treeView, &imageViewerPlugin, updateButtons](const QModelIndex& parent, int first, int last) {
        const auto index = treeView->model()->index(first, 0);
        
        if (index.isValid())
            imageViewerPlugin.getSelectionModel().select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        updateButtons();
    };

    connect(treeView->model(), &QAbstractListModel::rowsInserted, this, onRowsInserted);

    connect(&editLayersAction->getRemoveLayerAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin.getModel().removeLayer(selectedRows.first());
    });

    connect(&editLayersAction->getDuplicateLayerAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin.getModel().duplicateLayer(selectedRows.first());
    });

    connect(&editLayersAction->getMoveLayerToTopAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), -1000);

        imageViewerPlugin.getImageViewerWidget().update();
    });

    connect(&editLayersAction->getMoveLayerUpAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), -1);

        imageViewerPlugin.getImageViewerWidget().update();
    });

    connect(&editLayersAction->getMoveLayerDownAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), 1);

        imageViewerPlugin.getImageViewerWidget().update();
    });

    connect(&editLayersAction->getMoveLayerToBottomAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), 1000);
    });

    updateButtons();
}
