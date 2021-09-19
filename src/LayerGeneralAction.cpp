#include "LayerGeneralAction.h"
#include "LayerAction.h"

LayerGeneralAction::LayerGeneralAction(LayerAction& layerAction) :
    GroupAction(&layerAction),
    _layerAction(layerAction),
    _nameAction(this, "Name")
{
    setText("General");
}
