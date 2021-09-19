#include "LayerCommonAction.h"
#include "LayerAction.h"

LayerCommonAction::LayerCommonAction(LayerAction& layerAction) :
    GroupAction(&layerAction),
    _layerAction(layerAction)
{
    setText("Common");
}
