#include "GeneralAction.h"
#include "Layer.h"
#include "LayersAction.h"
#include "ImageViewerPlugin.h"

GeneralAction::GeneralAction(Layer& layer) :
    GroupAction(&layer, "General", true),
    _layer(layer),
    _visibleAction(this, "Visible", true, true),
    _datasetNameAction(this, "Dataset name"),
    _colorAction(this, "Color"),
    _nameAction(this, "Name"),
    _positionAction(*this),
    _scaleAction(this, "Scale", 0.0f, 1000000.0f, 100.0f, 100.0f, 1)
{
    addAction(&_visibleAction);
    addAction(&_datasetNameAction);
    addAction(&_colorAction);
    addAction(&_nameAction);
    addAction(&_positionAction);
    addAction(&_scaleAction);

    _datasetNameAction.setConnectionPermissionsToForceNone();
    _datasetNameAction.setEnabled(false);
    
    _scaleAction.setDefaultWidgetFlags(DecimalAction::SpinBox);

    _visibleAction.setToolTip("Visibility of the layer");
    _datasetNameAction.setToolTip("Name of the images dataset");
    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");

    const auto layerColor = _layer.getLayersAction().getRandomLayerColor();

    _colorAction.initialize(layerColor, layerColor),

    _scaleAction.setSuffix("%");

    const auto guiName = _layer.getImages()->getGuiName();

    _datasetNameAction.setString(guiName);

    _nameAction.setString(guiName);
    _nameAction.setDefaultString(guiName);

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
