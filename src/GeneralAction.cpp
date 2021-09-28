#include "GeneralAction.h"
#include "LayerAction.h"
#include "LayersAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

GeneralAction::GeneralAction(LayerAction& layerAction) :
    GroupAction(&layerAction, true),
    _layerAction(layerAction),
    _visibleAction(this, "Visible", true, true),
    _colorAction(this, "Color"),
    _nameAction(this, "Name"),
    _scaleAction(this, "Scale", 0.0f, 1000.0f, 100.0f, 100.0f, 1),
    _xPositionAction(this, "X position", -1000.0f, 1000.0f, 0.0f),
    _yPositionAction(this, "Y position", -1000.0f, 1000.0f, 0.0f),
    _zoomToExtentsAction(this, "Zoom to extents")
{
    setText("General");

    _visibleAction.setWidgetFlags(ToggleAction::CheckBoxAndResetPushButton);
    _colorAction.setWidgetFlags(ColorAction::All);
    _nameAction.setWidgetFlags(StringAction::All);
    _scaleAction.setWidgetFlags(DecimalAction::All);
    _xPositionAction.setWidgetFlags(DecimalAction::All);
    _yPositionAction.setWidgetFlags(DecimalAction::All);

    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");
    _xPositionAction.setToolTip("Layer x-position");
    _yPositionAction.setToolTip("Layer y-position");
    _zoomToExtentsAction.setToolTip("Zoom to the extents of the layer");

    // Get initial random layer color
    const auto layerColor = _layerAction.getLayersAction().getRandomLayerColor();

    // Assign the color and default color
    _colorAction.initialize(layerColor, layerColor),

    // Set scale suffix
    _scaleAction.setSuffix("%");

    // Get the name of the images dataset
    const auto imagesDatasetName = _layerAction.getLayer().getImagesDatasetName();

    // Set name and default name
    _nameAction.setString(imagesDatasetName);
    _nameAction.setDefaultString(imagesDatasetName);

    // Zoom to extents
    connect(&_zoomToExtentsAction, &ToggleAction::triggered, this, [this]() {
        _layerAction.getLayer().zoomToExtents();
    });

    const auto updateBounds = [this]() {
        _layerAction.getLayer().getImageViewerPlugin().getImageViewerWidget()->updateWorldBoundingRectangle();
    };

    connect(&_visibleAction, &ToggleAction::toggled, this, updateBounds);
    connect(&_scaleAction, &DecimalAction::valueChanged, this, updateBounds);
    connect(&_xPositionAction, &DecimalAction::valueChanged, this, updateBounds);
    connect(&_yPositionAction, &DecimalAction::valueChanged, this, updateBounds);

    updateBounds();
}
