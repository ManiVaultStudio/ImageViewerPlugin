#include "SelectionLayerAction.h"

SelectionLayerAction::SelectionLayerAction(LayersAction& layersAction) :
    LayerAction(&layersAction),
    _layersAction(layersAction),
    _commonLayerAction(this),
    _selectionLayerSettingsAction(*this),
    _image()
{
}
