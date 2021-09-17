#include "CommonLayerAction.h"

CommonLayerAction::CommonLayerAction(LayerAction* layerAction) :
    WidgetAction(reinterpret_cast<QObject*>(layerAction)),
    _layerAction(layerAction)
{
}
