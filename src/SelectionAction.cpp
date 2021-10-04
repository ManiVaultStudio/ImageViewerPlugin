#include "SelectionAction.h"
#include "LayerAction.h"
#include "Layer.h"

#include "util/PixelSelectionTool.h"

#include "Application.h"

using namespace hdps::util;

SelectionAction::SelectionAction(LayerAction& layerAction, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(&layerAction, targetWidget, pixelSelectionTool),
    _layerAction(layerAction),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _groupAction(this, true)
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
    _groupAction << _overlayColor;
    _groupAction << _overlayOpacity;
    _groupAction << _notifyDuringSelectionAction;

    const auto render = [this]() {
        _layerAction.getLayer().invalidate();
    };

    // Re-render when the overlay color or opacity changes
    connect(&_overlayColor, &ColorAction::colorChanged, this, render);
    connect(&_overlayOpacity, &DecimalAction::valueChanged, this, render);
}

QRect SelectionAction::getSelectionBoundaries() const
{
    return _layerAction.getImageAction().getChannelSelectionAction().getSelectionBoundaries();
}
