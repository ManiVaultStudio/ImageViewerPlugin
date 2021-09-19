#include "LayerImageAction.h"
#include "LayerAction.h"

LayerImageAction::LayerImageAction(LayerAction& layerAction) :
    GroupAction(&layerAction),
    _layerAction(layerAction)
{
    setText("Image");
}
