#include "LayersAction.h"
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

LayersAction::LayersAction(SettingsAction& settingsAction) :
    WidgetAction(reinterpret_cast<QObject*>(&settingsAction), "Layers"),
    _settingsAction(settingsAction),
    _currentLayerAction(this, "Groups"),
    _rng(0)
{
    setText("Layers");

    // Activate selected layer and deactivate any layer that was selected
    connect(&_settingsAction.getImageViewerPlugin().getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [](const QItemSelection& selected, const QItemSelection& deselected) {

        // Deactivate deselected layers
        if (!deselected.indexes().isEmpty()) {

            // Get pointer to layer that was deselected
            auto layer = static_cast<Layer*>(deselected.indexes().first().internalPointer());

            // Deactivate the layer
            layer->deactivate();
        }

        // Activate selected layers
        if (!selected.indexes().isEmpty()) {

            // Get pointer to layer that was selected
            auto layer = static_cast<Layer*>(selected.indexes().first().internalPointer());

            // Activate the layer
            layer->activate();
        }
    });
}

QColor LayersAction::getRandomLayerColor()
{
    // Randomize HSL parameters
    const auto randomHue        = _rng.bounded(360);
    const auto randomSaturation = _rng.bounded(150, 255);
    const auto randomLightness  = _rng.bounded(150, 220);

    // Create random color from hue, saturation and lightness
    return QColor::fromHsl(randomHue, randomSaturation, randomLightness);
}

LayersAction::Widget::Widget(QWidget* parent, LayersAction* layersAction) :
    WidgetActionWidget(parent, layersAction),
    _removeLayerAction(this, "Remove"),
    _duplicateLayerAction(this, "Duplicate"),
    _moveLayerToTopAction(this, "Move To Top"),
    _moveLayerUpAction(this, "Move Up"),
    _moveLayerDownAction(this, "Move Down"),
    _moveLayerToBottomAction(this, "Move To Bottom")
{
    auto& imageViewerPlugin = layersAction->getSettingsAction().getImageViewerPlugin();

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

    auto toolbarLayout = new QHBoxLayout();

    toolbarLayout->setSpacing(3);

    toolbarLayout->addWidget(_removeLayerAction.createWidget(this, TriggerAction::Icon));
    //toolbarLayout->addWidget(_duplicateLayerAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_moveLayerToTopAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addWidget(_moveLayerUpAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addWidget(_moveLayerDownAction.createWidget(this, TriggerAction::Icon));
    toolbarLayout->addWidget(_moveLayerToBottomAction.createWidget(this, TriggerAction::Icon));

    layout->addLayout(toolbarLayout);

    auto currentLayerWidget = layersAction->getCurrentLayerAction().createWidget(this);

    layout->addWidget(createHorizontalDivider());
    layout->addWidget(currentLayerWidget);

    setLayout(layout);

    const auto modelSelectionChanged = [this, layersAction, &imageViewerPlugin, treeView, layout]() -> void {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        GroupsAction::GroupActions groupActions;

        if (hasSelection) {
            auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

            groupActions << &layer->getGeneralAction() << &layer->getImageAction() << &layer->getSelectionAction() << &layer->getMiscellaneousAction();
        }

        layersAction->getCurrentLayerAction().setGroupActions(groupActions);

        // Enable the pixel selection tool when there is a selection and disable otherwise
        imageViewerPlugin.getImageViewerWidget().getPixelSelectionTool().setEnabled(hasSelection);
    };

    // Update various actions when the model is somehow changed (rows added/removed etc.)
    const auto updateButtons = [this, &imageViewerPlugin, treeView, layout]() -> void {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();
        const auto hasSelection = !selectedRows.isEmpty();

        _removeLayerAction.setEnabled(hasSelection);
        _duplicateLayerAction.setEnabled(hasSelection);

        auto selectedRowIndex = hasSelection ? selectedRows.first().row() : -1;

        _moveLayerToTopAction.setEnabled(hasSelection && selectedRowIndex > 0);
        _moveLayerUpAction.setEnabled(selectedRowIndex > 0 ? selectedRowIndex > 0 : false);
        _moveLayerDownAction.setEnabled(selectedRowIndex >= 0 ? selectedRowIndex < treeView->model()->rowCount() - 1 : false);
        _moveLayerToBottomAction.setEnabled(hasSelection && selectedRowIndex < treeView->model()->rowCount() - 1);

        // Render
        imageViewerPlugin.getImageViewerWidget().update();
    };

    connect(&imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, modelSelectionChanged);
    connect(&imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsRemoved, updateButtons);
    connect(treeView->model(), &QAbstractListModel::rowsMoved, updateButtons);
    connect(treeView->model(), &QAbstractListModel::layoutChanged, updateButtons);

    // Select a layer when it is inserted into the model
    const auto onRowsInserted = [treeView, &imageViewerPlugin, updateButtons](const QModelIndex& parent, int first, int last) {

        // Get model of inserted layer
        const auto index = treeView->model()->index(first, 0);
        
        // Select the layer if the index is valid
        if (index.isValid())
            imageViewerPlugin.getSelectionModel().select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        // Update layer position buttons
        updateButtons();
    };

    // Special behavior when a row is inserted into the model
    connect(treeView->model(), &QAbstractListModel::rowsInserted, this, onRowsInserted);

    // Remove the layer when the corresponding action is triggered
    connect(&_removeLayerAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin.getModel().removeLayer(selectedRows.first());
    });

    // Duplicate the layer when the corresponding action is triggered
    connect(&_duplicateLayerAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (selectedRows.isEmpty())
            return;

        imageViewerPlugin.getModel().duplicateLayer(selectedRows.first());
    });

    // Move the layer to the top when the corresponding action is triggered
    connect(&_moveLayerToTopAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), -1000);

        // Render
        imageViewerPlugin.getImageViewerWidget().update();
    });

    // Move the layer up when the corresponding action is triggered
    connect(&_moveLayerUpAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), -1);

        // Render
        imageViewerPlugin.getImageViewerWidget().update();
    });

    // Move the layer down when the corresponding action is triggered
    connect(&_moveLayerDownAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), 1);

        // Render
        imageViewerPlugin.getImageViewerWidget().update();
    });

    // Move the layer to the bottom when the corresponding action is triggered
    connect(&_moveLayerToBottomAction, &TriggerAction::triggered, this, [this, &imageViewerPlugin, treeView]() {
        const auto selectedRows = imageViewerPlugin.getSelectionModel().selectedRows();

        if (!selectedRows.isEmpty())
            imageViewerPlugin.getModel().moveLayer(selectedRows.first(), 1000);
    });

    updateButtons();
    modelSelectionChanged();
}
