#include "GeneralAction.h"
#include "Layer.h"
#include "LayersAction.h"
#include "ImageViewerPlugin.h"

GeneralAction::GeneralAction(Layer& layer) :
    GroupAction(&layer, true),
    _layer(layer),
    _visibleAction(this, "Visible", true, true),
    _datasetNameAction(this, "Dataset name"),
    _colorAction(this, "Color"),
    _nameAction(this, "Name"),
    _positionAction(*this),
    _scaleAction(this, "Scale", 0.0f, 1000.0f, 100.0f, 100.0f, 1),
    _zoomAction(*this)
{
    setText("General");

    _datasetNameAction.setEnabled(false);
    _datasetNameAction.setMayReset(false);

    // Set tooltips
    _visibleAction.setToolTip("Visibility of the layer");
    _datasetNameAction.setToolTip("Name of the images dataset");
    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");

    // Get initial random layer color
    const auto layerColor = _layer.getLayersAction().getRandomLayerColor();

    // Assign the color and default color
    _colorAction.initialize(layerColor, layerColor),

    // Set scale suffix
    _scaleAction.setSuffix("%");

    // Get the name of the images dataset
    const auto guiName = _layer.getImages()->getGuiName();

    // Set dataset name
    _datasetNameAction.setString(guiName);

    // Set layer name and default name
    _nameAction.setString(guiName);
    _nameAction.setDefaultString(guiName);

    // Zoom to extents
    connect(&_zoomAction, &ToggleAction::triggered, this, [this]() {
        _layer.zoomToExtents();
    });

    const auto render = [this]() {
        _layer.getImageViewerPlugin().getImageViewerWidget().update();

    };
    const auto updateBounds = [this]() {
        _layer.getImageViewerPlugin().getImageViewerWidget().updateWorldBoundingRectangle();
    };

    connect(&_nameAction, &StringAction::stringChanged, this, render);
    connect(&_visibleAction, &ToggleAction::toggled, this, updateBounds);
    connect(&_positionAction, &PositionAction::changed, this, updateBounds);
    connect(&_scaleAction, &DecimalAction::valueChanged, this, updateBounds);
    connect(&_colorAction, &ColorAction::colorChanged, this, updateBounds);
    connect(&_colorAction, &ColorAction::colorChanged, this, render);

    updateBounds();
}
