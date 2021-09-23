#include "LayerAction.h"
#include "ImageViewerPlugin.h"

LayerAction::LayerAction(Layer& layer) :
    WidgetAction(reinterpret_cast<QObject*>(&layer)),
    _layer(layer),
    _generalAction(*this),
    _imageAction(*this),
    _selectionAction(layer.getImageViewerPlugin(), layer.getImageViewerPlugin()->getImageViewerWidget()->getPixelSelectionTool())
{
}

Layer& LayerAction::getLayer()
{
    return _layer;
}
