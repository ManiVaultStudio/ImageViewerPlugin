#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "SelectionToolbarAction.h"
#include "InteractionToolbarAction.h"

#include <QMenu>

using namespace mv::gui;

SettingsAction::SettingsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _imageViewerPlugin(*static_cast<ImageViewerPlugin*>(parent)),
    _editLayersAction(this, "Edit Layers"),
    _editLayerAction(this, "Edit Layer")
{
    setShowLabels(false);

    addAction(&_editLayersAction);
    addAction(&_editLayerAction);

    _editLayerAction.setStretch(1);
}

QMenu* SettingsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    auto menu = new QMenu();

    const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

    if (!selectedRows.isEmpty()) {
        auto layer = _imageViewerPlugin.getLayersModel().getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return nullptr;

        auto& selectionAction = layer->getSelectionAction();

        auto selectionTypesWidget = new QWidget();
        auto selectionTypesLayout = new QHBoxLayout();

        selectionTypesLayout->setContentsMargins(0, 0, 0, 0);
        selectionTypesLayout->setSpacing(2);

        selectionTypesWidget->setLayout(selectionTypesLayout);

        auto& pixelSelectionAction = selectionAction.getPixelSelectionAction();

        auto& interactionToolbar = _imageViewerPlugin.getInteractionToolbarAction();
        auto& selectionToolbar   = _imageViewerPlugin.getSelectionToolbarAction();

        selectionTypesLayout->addWidget(interactionToolbar.getSelectAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(interactionToolbar.getNavigationAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addStretch(1);
        selectionTypesLayout->addWidget(pixelSelectionAction.getRectangleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getBrushAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getLassoAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getPolygonAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getSampleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));

        auto selectionTypeWidgetAction = new QWidgetAction(this);

        selectionTypeWidgetAction->setDefaultWidget(selectionTypesWidget);

        menu->addAction(selectionTypeWidgetAction);
    }

    menu->addSeparator();

    for (auto layer : _imageViewerPlugin.getLayersModel().getLayers()) {
        auto layerAction = new QAction(layer->getGeneralAction().getNameAction().getString());

        layerAction->setCheckable(true);
        layerAction->setChecked(layer->getGeneralAction().getVisibleAction().isChecked());

        connect(layerAction, &QAction::toggled, this, [this, layer](bool toggled) {
            layer->getGeneralAction().getVisibleAction().setChecked(toggled);
        });

        menu->addAction(layerAction);
    }

    menu->addSeparator();

    menu->addAction(&_imageViewerPlugin.getInteractionToolbarAction().getZoomExtentsAction());
    menu->addAction(&_imageViewerPlugin.getInteractionToolbarAction().getZoomSelectionAction());
    menu->addAction(&_imageViewerPlugin.getInteractionToolbarAction().getUpdateSelectionROI());

    return menu;
}

