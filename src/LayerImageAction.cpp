#include "LayerImageAction.h"
#include "LayerAction.h"
#include "Layer.h"

using namespace hdps::util;

LayerImageAction::LayerImageAction(LayerAction& layerAction) :
    GroupAction(&layerAction, true),
    _layerAction(layerAction),
    _opacityAction(this, "Opacity", 0.0f, 100.0f, 100.0f, 100.0f, 1),
    _channel1Action(*this, "Channel 1"),
    _channel2Action(*this, "Channel 2"),
    _channel3Action(*this, "Channel 3"),
    _colorSpaceAction(this, "Color space", {"RGB", "HSV", "LAB"}, "RGB", "RGB"),
    _colorMapAction(this, "Color map"),
    _interpolationTypeAction(this, "Interpolation type", {"Bilinear", "Nearest neighbor"}, "Bilinear", "Bilinear"),
    _constantColorAction(this, "Constant color", true, true)
{
    setText("Image");

    _opacityAction.setToolTip("Image layer opacity");
    _channel1Action.setToolTip("Channel 1");
    _channel2Action.setToolTip("Channel 2");
    _channel3Action.setToolTip("Channel 3");
    _colorSpaceAction.setToolTip("The color space used to shade the image");
    _colorMapAction.setToolTip("Image color map");
    _interpolationTypeAction.setToolTip("The type of two-dimensional image interpolation used");

    _opacityAction.setSuffix("%");

    _channel1Action.setWidgetFlags(ChannelAction::All);
    _channel2Action.setWidgetFlags(ChannelAction::All);
    _channel3Action.setWidgetFlags(ChannelAction::All);
    _colorSpaceAction.setWidgetFlags(OptionAction::All);
    _colorMapAction.setWidgetFlags(ColorMapAction::Settings | ColorMapAction::ResetPushButton);
    _interpolationTypeAction.setWidgetFlags(OptionAction::All);

    _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

    _channel1Action.getEnabledAction().setEnabled(false);

    const auto dimensionNames = _layerAction.getLayer().getDimensionNames();

    _channel1Action.getDimensionAction().setOptions(dimensionNames);
    _channel2Action.getDimensionAction().setOptions(dimensionNames);
    _channel3Action.getDimensionAction().setOptions(dimensionNames);
}
