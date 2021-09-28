#include "LayerAction.h"
#include "LayersAction.h"

LayerAction::LayerAction(Layer& layer, LayersAction& layersAction) :
    WidgetAction(reinterpret_cast<QObject*>(&layer)),
    _layer(layer),
    _layersAction(layersAction),
    _generalAction(*this),
    _imageAction(*this)
{
    const auto updateActions = [this]() -> void {
        _imageAction.setEnabled(_generalAction.getVisibleAction().isChecked());
    };

    //connect(&_generalAction.getVisibleAction(), &ToggleAction::toggled, this, updateActions);

    updateActions();
}

Layer& LayerAction::getLayer()
{
    return _layer;
}