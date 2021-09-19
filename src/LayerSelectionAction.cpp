#include "LayerSelectionAction.h"
#include "LayerAction.h"

LayerSelectionAction::LayerSelectionAction(LayerAction& layerAction) :
    GroupAction(&layerAction),
    _layerAction(layerAction),
    _image()
{
    setText("Selection");
}
