#include "LayerAction.h"

LayerAction::LayerAction(Layer& layer) :
    WidgetAction(reinterpret_cast<QObject*>(&layer)),
    _layer(layer),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(*this)
{
}
