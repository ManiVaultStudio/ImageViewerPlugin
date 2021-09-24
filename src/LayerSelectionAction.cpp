#include "LayerSelectionAction.h"
#include "LayerAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

#include "Application.h"

#include "util/PixelSelectionTool.h"

LayerSelectionAction::LayerSelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(targetWidget, pixelSelectionTool),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _createSubsetFromSelectionAction(this, "Create subset")
{
    setText("Layer selection");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
}
