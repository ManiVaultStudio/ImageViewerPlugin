#include "LayerGeneralAction.h"
#include "LayerAction.h"
#include "Layer.h"

LayerGeneralAction::LayerGeneralAction(LayerAction& layerAction) :
    GroupAction(&layerAction),
    _layerAction(layerAction),
    _nameAction(this, "Name"),
    _scaleAction(this, "Scale", 0.0f, 100.0f, 100.0f, 100.0f, 1)
{
    setText("General");

    _nameAction.setWidgetFlags(StringAction::WidgetFlag::All);
    _scaleAction.setWidgetFlags(DecimalAction::WidgetFlag::All);

    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");

    _scaleAction.setSuffix("%");

    const auto imagesDatasetName = _layerAction.getLayer().getImagesDatasetName();

    _nameAction.setString(imagesDatasetName);
    _nameAction.setDefaultString(imagesDatasetName);
}
