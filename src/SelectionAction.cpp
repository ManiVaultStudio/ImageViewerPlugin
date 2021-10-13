#include "SelectionAction.h"
#include "Layer.h"

#include "util/PixelSelectionTool.h"

#include "Application.h"

using namespace hdps::util;

SelectionAction::SelectionAction(Layer& layer, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(&layer, targetWidget, pixelSelectionTool),
    _layer(layer),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _showRegionAction(this, "Show selected region", true, true),
    _groupAction(this, false)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    auto allowedPixelSelectionTypes = defaultPixelSelectionTypes;

    // Add 'sample 'pixel selection type
    allowedPixelSelectionTypes << PixelSelectionType::Sample;

    // Assign allowed types
    setAllowedTypes(allowedPixelSelectionTypes);

    _groupAction.setText("Selection");

    // Populate group action
    _groupAction << _typeAction;
    _groupAction << _brushRadiusAction;
    _groupAction << _showRegionAction;
    _groupAction << _overlayColor;
    _groupAction << _overlayOpacity;
    _groupAction << _notifyDuringSelectionAction;

    // Re-render
    const auto render = [this]() {
        _layer.invalidate();
    };

    // Re-render when the overlay color, overlay opacity or show region changes
    connect(&_overlayColor, &ColorAction::colorChanged, this, render);
    connect(&_overlayOpacity, &DecimalAction::valueChanged, this, render);
    connect(&_showRegionAction, &ToggleAction::toggled, this, render);
}

QRect SelectionAction::getSelectionBoundaries() const
{
    return _layer.getImageAction().getChannelSelectionAction().getSelectionBoundaries();
}
