#include "LayerAction.h"
#include "LayersAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

LayerAction::LayerAction(Layer& layer, LayersAction& layersAction) :
    WidgetAction(reinterpret_cast<QObject*>(&layer)),
    _layer(layer),
    _layersAction(layersAction),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(*this, layer.getImageViewerPlugin().getImageViewerWidget(), layer.getImageViewerPlugin().getImageViewerWidget()->getPixelSelectionTool()),
    _subsetAction(*this)
{
    const auto updateActions = [this]() -> void {

        // Determine whether the layer is visible
        const auto layerIsVisible = _generalAction.getVisibleAction().isChecked();

        // Enable/disable image and selection groups
        //_imageAction.setEnabled(layerIsVisible);
        //_selectionAction.getGroupAction().setEnabled(layerIsVisible);
        //_subsetAction.setEnabled(layerIsVisible);
    };

    connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, updateActions);

    updateActions();
}

Layer& LayerAction::getLayer()
{
    return _layer;
}