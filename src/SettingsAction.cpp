#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "MainToolbarAction.h"
#include "NavigationAction.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _layersAction(*this)
{
}

QMenu* SettingsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu();

    // Get selected rows from the selection model
    const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

    // Show selection types when a row is selected
    if (!selectedRows.isEmpty()) {

        // Get the layer from the row index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Get the selection action
        auto& selectionAction = layer->getSelectionAction();

        // Create container widget
        auto selectionTypesWidget = new QWidget();

        // Create horizontal layout for the selection types
        auto selectionTypesLayout = new QHBoxLayout();

        // Remove margins and tighten space between buttons
        selectionTypesLayout->setMargin(0);
        selectionTypesLayout->setSpacing(2);

        // Apply the layout
        selectionTypesWidget->setLayout(selectionTypesLayout);

        // Add selection type buttons to the layout
        selectionTypesLayout->addWidget(selectionAction.getRectangleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(selectionAction.getBrushAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(selectionAction.getLassoAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(selectionAction.getPolygonAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(selectionAction.getSampleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addStretch(1);

        // Create widget action that will display the selection types widget
        auto selectionTypeWidgetAction = new QWidgetAction(this);

        // Assign the selection types widget to the widget action 
        selectionTypeWidgetAction->setDefaultWidget(selectionTypesWidget);

        // Enabled if the layer is visible
        selectionTypeWidgetAction->setEnabled(layer->getGeneralAction().getVisibleAction().isChecked() && _imageViewerPlugin.getImageViewerWidget().getInteractionMode() == ImageViewerWidget::Selection);

        // Add widget action to the menu
        menu->addAction(selectionTypeWidgetAction);
    }

    menu->addSeparator();

    // Add layer actions (show/hide) to the menu
    for (auto layer : _imageViewerPlugin.getModel().getLayers()) {

        // Create action for turning layers on/off
        auto layerAction = new QAction(layer->getGeneralAction().getNameAction().getString());

        // Make the menu item checkable and initialize the checked state
        layerAction->setCheckable(true);
        layerAction->setChecked(layer->getGeneralAction().getVisibleAction().isChecked());

        // Show/hide the layer when the menu item is toggled
        connect(layerAction, &QAction::toggled, this, [this, layer](bool toggled) {
            layer->getGeneralAction().getVisibleAction().setChecked(toggled);
        });

        // Add the layer action to the menu
        menu->addAction(layerAction);
    }

    menu->addSeparator();

    menu->addAction(&_imageViewerPlugin.getMainToolbarAction().getPanAction());
    menu->addAction(&_imageViewerPlugin.getMainToolbarAction().getSelectAction());
    menu->addAction(&_imageViewerPlugin.getNavigationAction().getZoomExtentsAction());

    return menu;
}

SettingsAction::Widget::Widget(QWidget* parent, SettingsAction* settingsAction) :
    WidgetActionWidget(parent, settingsAction)
{
    setAutoFillBackground(true);
    setFixedWidth(350);
    
    auto layout = new QVBoxLayout();

    layout->setMargin(4);
    layout->addWidget(settingsAction->getLayersAction().createWidget(this));

    setLayout(layout);
}
