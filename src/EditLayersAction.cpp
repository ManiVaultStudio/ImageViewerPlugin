#include "EditLayersAction.h"
#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "SettingsAction.h"

#include <Application.h>

#include <widgets/Divider.h>

#include <QTreeView>
#include <QHeaderView>

using namespace mv;
using namespace mv::gui;

QRandomGenerator EditLayersAction::rng;

EditLayersAction::EditLayersAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _settingsAction(*static_cast<SettingsAction*>(parent)),
    _removeLayerAction(this, "Remove"),
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
    _operationsAction.addStretch(1);

    _removeLayerAction.setToolTip("Remove the selected layer");
    _moveLayerToTopAction.setToolTip("Move the selected layer to the top");
    _moveLayerUpAction.setToolTip("Move the selected layer up");
    _moveLayerDownAction.setToolTip("Move the selected layer down");
    _moveLayerToBottomAction.setToolTip("Move the selected layer to the bottom");

    _removeLayerAction.setIconByName("trash-alt");
    _moveLayerToTopAction.setIconByName("angle-double-up");
    _moveLayerUpAction.setIconByName("angle-up");
    _moveLayerDownAction.setIconByName("angle-down");
    _moveLayerToBottomAction.setIconByName("angle-double-down");

    connect(&_settingsAction.getImageViewerPlugin().getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& selected, const QItemSelection& deselected) {
        auto& layersModel = _settingsAction.getImageViewerPlugin().getLayersModel();

        if (!deselected.indexes().isEmpty()) {
            auto layer = layersModel.getLayerFromIndex(deselected.indexes().first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

            layer->deactivate();
        }

        if (!selected.indexes().isEmpty()) {
            auto layer = layersModel.getLayerFromIndex(selected.indexes().first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

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
    WidgetActionWidget(parent, editLayersAction),
    _hierarchyWidget(this, "Layer", editLayersAction->getSettingsAction().getImageViewerPlugin().getLayersModel(), nullptr, false)
{
    auto& imageViewerPlugin = editLayersAction->getSettingsAction().getImageViewerPlugin();

    _hierarchyWidget.setWindowIcon(StyledIcon("layer-group"));
    
    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setSelectionModel(&imageViewerPlugin.getSelectionModel());
    treeView.setRootIsDecorated(false);
    treeView.setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SelectionMode::ContiguousSelection);
    
    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeView.setColumnHidden(static_cast<int>(LayersModel::Column::DatasetId), true);

    treeViewHeader->setSectionResizeMode(static_cast<int>(LayersModel::Column::Visible), QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(static_cast<int>(LayersModel::Column::Color), QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(static_cast<int>(LayersModel::Column::Name), QHeaderView::Stretch);

    const auto toggleColumnSize = 16;

    treeViewHeader->resizeSection(static_cast<int>(LayersModel::Column::Visible), toggleColumnSize);
    treeViewHeader->resizeSection(static_cast<int>(LayersModel::Column::Color), toggleColumnSize);

    connect(&_hierarchyWidget.getTreeView(), &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        static const QVector<int> toggleColumns = {
            static_cast<int>(LayersModel::Column::Visible)
        };

        if (!toggleColumns.contains(index.column()))
            return;

        const auto sourceModelIndex = _hierarchyWidget.toSourceModelIndex(index);

        auto& actionsModel = const_cast<QAbstractItemModel&>(_hierarchyWidget.getModel());

        actionsModel.setData(sourceModelIndex, !actionsModel.data(sourceModelIndex, Qt::EditRole).toBool(), Qt::EditRole);
    });

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_hierarchyWidget);
    layout->addWidget(editLayersAction->getOperationsAction().createWidget(this));

    setLayout(layout);

    const auto updateButtons = [this, &imageViewerPlugin, editLayersAction]() -> void {
        const auto selectedRows     = imageViewerPlugin.getSelectionModel().selectedRows();
        const auto hasSelection     = !selectedRows.isEmpty();
        const auto multiSelection   = selectedRows.count() >= 2;

        editLayersAction->getRemoveLayerAction().setEnabled(hasSelection);

        auto selectedRowIndex = hasSelection ? selectedRows.first().row() : -1;

        editLayersAction->getMoveLayerToTopAction().setEnabled(!multiSelection && hasSelection && selectedRowIndex > 0);
        editLayersAction->getMoveLayerUpAction().setEnabled(!multiSelection && selectedRowIndex > 0 ? selectedRowIndex > 0 : false);
        editLayersAction->getMoveLayerDownAction().setEnabled(!multiSelection && selectedRowIndex >= 0 ? selectedRowIndex < _hierarchyWidget.getModel().rowCount() - 1 : false);
        editLayersAction->getMoveLayerToBottomAction().setEnabled(!multiSelection && hasSelection && selectedRowIndex < _hierarchyWidget.getModel().rowCount() - 1);

        imageViewerPlugin.getImageViewerWidget().update();
    };

    connect(&imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateButtons);
    connect(&_hierarchyWidget.getModel(), &QAbstractListModel::rowsRemoved, updateButtons);
    connect(&_hierarchyWidget.getModel(), &QAbstractListModel::rowsMoved, updateButtons);
    connect(&_hierarchyWidget.getModel(), &QAbstractListModel::layoutChanged, updateButtons);

    const auto onRowsInserted = [this, &imageViewerPlugin, updateButtons](const QModelIndex& parent, int first, int last) {
        if (projects().isOpeningProject() || projects().isImportingProject())
            return;

        const auto index = _hierarchyWidget.getModel().index(first, 0);
        
        if (index.isValid())
            imageViewerPlugin.getSelectionModel().select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        updateButtons();
    };

    connect(&_hierarchyWidget.getModel(), &QAbstractListModel::rowsInserted, this, onRowsInserted);

    connect(&editLayersAction->getRemoveLayerAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.isEmpty())
            return;

        QVector<QPersistentModelIndex> persistentSelectedRows;

        for (auto selectedRow : selectedRows)
            persistentSelectedRows << selectedRow;

        for (auto persistentSelectedRow : persistentSelectedRows)
            imageViewerPlugin.getLayersModel().removeLayer(persistentSelectedRow);
    });

    connect(&editLayersAction->getMoveLayerToTopAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;

        imageViewerPlugin.getLayersModel().moveLayer(selectedRows.first(), -1000);
    });

    connect(&editLayersAction->getMoveLayerUpAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;
    
        imageViewerPlugin.getLayersModel().moveLayer(selectedRows.first(), -1);
    });

    connect(&editLayersAction->getMoveLayerDownAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;
        
        imageViewerPlugin.getLayersModel().moveLayer(selectedRows.first(), 1);
    });

    connect(&editLayersAction->getMoveLayerToBottomAction(), &TriggerAction::triggered, this, [this, &imageViewerPlugin]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;
        
        imageViewerPlugin.getLayersModel().moveLayer(selectedRows.first(), 1000);
    });

    updateButtons();
}
