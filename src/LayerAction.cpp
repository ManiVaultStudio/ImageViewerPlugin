#include "LayerAction.h"

LayerAction::LayerAction(LayersAction* layersAction) :
    WidgetAction(reinterpret_cast<QObject*>(layersAction)),
    _layersAction(layersAction)
{
}
