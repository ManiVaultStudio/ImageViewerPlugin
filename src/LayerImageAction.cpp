#include "LayerImageAction.h"
#include "LayerAction.h"
#include "Layer.h"

#include "util/ColorSpace.h"
#include "util/Interpolation.h"

using namespace hdps;
using namespace hdps::util;

LayerImageAction::LayerImageAction(LayerAction& layerAction) :
    GroupAction(&layerAction, true),
    _layerAction(layerAction),
    _opacityAction(this, "Opacity", 0.0f, 100.0f, 100.0f, 100.0f, 1),
    _colorSpaceAction(this, "Color space", colorSpaces.values(), "Mono", "Mono"),
    _channel1Action(*this, ChannelAction::Channel1, ChannelAction::channelIndexes.value(ChannelAction::Channel1)),
    _channel2Action(*this, ChannelAction::Channel2, ChannelAction::channelIndexes.value(ChannelAction::Channel2)),
    _channel3Action(*this, ChannelAction::Channel3, ChannelAction::channelIndexes.value(ChannelAction::Channel3)),
    _channelMaskAction(*this, ChannelAction::Mask, ChannelAction::channelIndexes.value(ChannelAction::Mask)),
    _channelSelectionAction(*this, ChannelAction::Selection, ChannelAction::channelIndexes.value(ChannelAction::Selection)),
    _colorMapAction(this, "Color map"),
    _interpolationTypeAction(this, "Interpolate", interpolationTypes.values(), "Bilinear", "Bilinear"),
    _constantColorAction(this, "Constant color", true, true)
{
    setText("Image");

    _channelMaskAction.setVisible(false);
    _channelSelectionAction.setVisible(false);

    _opacityAction.setToolTip("Image layer opacity");
    _channel1Action.setToolTip("Channel 1");
    _channel2Action.setToolTip("Channel 2");
    _channel3Action.setToolTip("Channel 3");
    _channelMaskAction.setToolTip("Mask channel");
    _colorSpaceAction.setToolTip("The color space used to shade the image");
    _colorMapAction.setToolTip("Image color map");
    _interpolationTypeAction.setToolTip("The type of two-dimensional image interpolation used");

    _opacityAction.setSuffix("%");

    _opacityAction.setWidgetFlags(DecimalAction::All);
    _channel1Action.setWidgetFlags(ChannelAction::All);
    _channel2Action.setWidgetFlags(ChannelAction::All);
    _channel3Action.setWidgetFlags(ChannelAction::All);
    _colorSpaceAction.setWidgetFlags(OptionAction::All);
    _colorMapAction.setWidgetFlags(ColorMapAction::Settings | ColorMapAction::ResetPushButton);
    _interpolationTypeAction.setWidgetFlags(OptionAction::All);
    _constantColorAction.setWidgetFlags(ToggleAction::CheckBoxAndResetPushButton);

    _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

    const auto dimensionNames = _layerAction.getLayer().getDimensionNames();

    _channel1Action.getDimensionAction().setOptions(dimensionNames);
    _channel2Action.getDimensionAction().setOptions(dimensionNames);
    _channel3Action.getDimensionAction().setOptions(dimensionNames);

    _channel1Action.getDimensionAction().setCurrentIndex(0);
    _channel1Action.getDimensionAction().setDefaultIndex(0);

    if (_layerAction.getLayer().getNumberOfImages() >= 2) {
        _channel2Action.getDimensionAction().setCurrentIndex(1);
        _channel2Action.getDimensionAction().setDefaultIndex(1);
    }

    if (_layerAction.getLayer().getNumberOfImages() >= 3) {
        _channel3Action.getDimensionAction().setCurrentIndex(2);
        _channel3Action.getDimensionAction().setDefaultIndex(2);
    }

    const auto updateChannelActions = [this]() -> void {
        switch (static_cast<ColorSpace>(_colorSpaceAction.getCurrentIndex()))
        {
            case ColorSpace::Mono:
                _channel1Action.getEnabledAction().setCheckable(true);
                _channel2Action.getEnabledAction().setCheckable(false);
                _channel3Action.getEnabledAction().setCheckable(false);
                _channel1Action.setText("Channel 1");
                _channel2Action.setText("Channel 2");
                _channel3Action.setText("Channel 3");
                _colorMapAction.setEnabled(true);
                _colorMapAction.setColorMapType(ColorMap::Type::OneDimensional);
                break;

            case ColorSpace::Duo:
                _channel1Action.getEnabledAction().setCheckable(true);
                _channel2Action.getEnabledAction().setCheckable(true);
                _channel3Action.getEnabledAction().setCheckable(false);
                _channel1Action.setText("Channel 1");
                _channel2Action.setText("Channel 2");
                _channel3Action.setText("Channel 3");
                _colorMapAction.setEnabled(true);
                _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);
                break;

            case ColorSpace::RGB:
                _channel1Action.getEnabledAction().setCheckable(true);
                _channel2Action.getEnabledAction().setCheckable(true);
                _channel3Action.getEnabledAction().setCheckable(true);
                _channel1Action.setText("Red");
                _channel2Action.setText("Green");
                _channel3Action.setText("Blue");
                _colorMapAction.setEnabled(false);
                break;

            case ColorSpace::HSL:
                _channel1Action.getEnabledAction().setCheckable(true);
                _channel2Action.getEnabledAction().setCheckable(true);
                _channel3Action.getEnabledAction().setCheckable(true);
                _channel1Action.setText("Hue");
                _channel2Action.setText("Saturation");
                _channel3Action.setText("Lightness");
                _colorMapAction.setEnabled(false);
                break;

            case ColorSpace::LAB:
                _channel1Action.getEnabledAction().setCheckable(true);
                _channel2Action.getEnabledAction().setCheckable(true);
                _channel3Action.getEnabledAction().setCheckable(true);
                _channel1Action.setText("L");
                _channel2Action.setText("A");
                _channel3Action.setText("B");
                _colorMapAction.setEnabled(false);
                break;

            default:
                break;
        }
    };

    connect(&_colorSpaceAction, &OptionAction::currentIndexChanged, this, updateChannelActions);

    updateChannelActions();
}
