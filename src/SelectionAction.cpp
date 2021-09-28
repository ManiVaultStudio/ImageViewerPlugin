#include "SelectionAction.h"
#include "LayerAction.h"
#include "Layer.h"

#include "actions/PixelSelectionTypeAction.h"
#include "util/PixelSelectionTool.h"

#include "Application.h"

using namespace hdps::util;

SelectionAction::SelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(targetWidget, pixelSelectionTool),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _createSubsetFromSelectionAction(this, "Create subset")
{
    setText("Layer selection");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    auto allowedPixelSelectionTypes = defaultPixelSelectionTypes;

    // Add 'sample 'pixel selection type
    allowedPixelSelectionTypes << PixelSelectionType::Sample;

    getPixelSelectionTypeAction().setAllowedTypes(allowedPixelSelectionTypes);
}
