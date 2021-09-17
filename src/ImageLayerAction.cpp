#include "ImageLayerAction.h"

ImageLayerAction::ImageLayerAction(LayersAction& layersAction) :
    LayerAction(&layersAction),
    _layersAction(layersAction),
    _commonLayerAction(this)
{
}
