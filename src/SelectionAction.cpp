#include "SelectionAction.h"
#include "Layer.h"

#include "util/PixelSelectionTool.h"

#include "Application.h"

using namespace hdps::util;

const auto allowedPixelSelectionTypes = PixelSelectionTypes({
    PixelSelectionType::Rectangle,
    PixelSelectionType::Brush,
    PixelSelectionType::Lasso,
    PixelSelectionType::Polygon,
    PixelSelectionType::Sample,
    PixelSelectionType::ROI
});

SelectionAction::SelectionAction(Layer& layer, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    PixelSelectionAction(&layer, targetWidget, pixelSelectionTool, allowedPixelSelectionTypes),
    _layer(layer),
    _targetWidget(targetWidget),
    _pixelSelectionTool(pixelSelectionTool),
    _showRegionAction(this, "Show selected region", false, false),
    _groupAction(this, false)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _groupAction.setText("Selection");

    // Populate group action
    _groupAction << _typeAction;
    _groupAction << _brushRadiusAction;
    _groupAction << _overlayColor;
    _groupAction << _overlayOpacity;
    _groupAction << _notifyDuringSelectionAction;

    // Re-render when the overlay color, overlay opacity or show region changes
    connect(&_overlayColor, &ColorAction::colorChanged, &_layer, &Layer::invalidate);
    connect(&_overlayOpacity, &DecimalAction::valueChanged, &_layer, &Layer::invalidate);
    connect(&_showRegionAction, &ToggleAction::toggled, &_layer, &Layer::invalidate);
}

QRect SelectionAction::getImageSelectionRectangle() const
{
    return _layer.getImageSelectionRectangle();
}
