#include "LayerGeneralAction.h"
#include "LayerAction.h"
#include "Layer.h"

LayerGeneralAction::LayerGeneralAction(LayerAction& layerAction) :
    GroupAction(&layerAction, true),
    _layerAction(layerAction),
    _visibleAction(this, "Visible", true, true),
    _colorAction(this, "Color"),
    _nameAction(this, "Name"),
    _scaleAction(this, "Scale", 0.0f, 100.0f, 100.0f, 100.0f, 1),
    _zoomToExtentsAction(this, "Zoom to extents")
{
    setText("General");

    _visibleAction.setWidgetFlags(ToggleAction::CheckBoxAndResetPushButton);
    _colorAction.setWidgetFlags(ColorAction::All);
    _nameAction.setWidgetFlags(StringAction::WidgetFlag::All);
    _scaleAction.setWidgetFlags(DecimalAction::WidgetFlag::All);

    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");
    _zoomToExtentsAction.setToolTip("Zoom to the extents of the layer");

    _scaleAction.setSuffix("%");

    const auto imagesDatasetName = _layerAction.getLayer().getImagesDatasetName();

    _nameAction.setString(imagesDatasetName);
    _nameAction.setDefaultString(imagesDatasetName);
}
