#include "SettingsAction.h"
#include "ImageViewerPlugin.h"
#include "SelectionToolbarAction.h"
#include "InteractionToolbarAction.h"

#include <QMenu>

using namespace hdps::gui;

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
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());
        auto& selectionAction = layer->getSelectionAction();

        auto selectionTypesWidget = new QWidget();
        auto selectionTypesLayout = new QHBoxLayout();

        selectionTypesLayout->setContentsMargins(0, 0, 0, 0);
        selectionTypesLayout->setSpacing(2);

        selectionTypesWidget->setLayout(selectionTypesLayout);

        auto& pixelSelectionAction = selectionAction.getPixelSelectionAction();

        selectionTypesLayout->addWidget(pixelSelectionAction.getRectangleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getBrushAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getLassoAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getPolygonAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addWidget(pixelSelectionAction.getSampleAction().createWidget(selectionTypesWidget, ToggleAction::PushButtonIcon));
        selectionTypesLayout->addStretch(1);

        auto selectionTypeWidgetAction = new QWidgetAction(this);

        selectionTypeWidgetAction->setDefaultWidget(selectionTypesWidget);

        selectionTypeWidgetAction->setEnabled(layer->getGeneralAction().getVisibleAction().isChecked() && _imageViewerPlugin.getImageViewerWidget().getInteractionMode() == ImageViewerWidget::Selection);

        menu->addAction(selectionTypeWidgetAction);
    }

    menu->addSeparator();

    for (auto layer : _imageViewerPlugin.getModel().getLayers()) {
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

    return menu;
}

