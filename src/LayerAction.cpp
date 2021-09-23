#include "LayerAction.h"
#include "ImageViewerPlugin.h"

LayerAction::LayerAction(Layer& layer) :
    WidgetAction(reinterpret_cast<QObject*>(&layer)),
    _layer(layer),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(layer.getImageViewerPlugin(), layer.getImageViewerPlugin()->getImageViewerWidget()->getPixelSelectionTool())
{
    const auto updateActions = [this]() -> void {
        const auto layerIsVisible = _generalAction.getVisibleAction().isChecked();

        _imageAction.setEnabled(layerIsVisible);
        _selectionAction.setEnabled(layerIsVisible);
    };

    connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, [this, updateActions]() {
        updateActions();
    });

    updateActions();
}

Layer& LayerAction::getLayer()
{
    return _layer;
}
