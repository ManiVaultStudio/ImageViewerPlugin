#include "LayerImageAction.h"
#include "LayerAction.h"
#include "Layer.h"
#include "Common.h"

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

    _opacityAction.setWidgetFlags(ChannelAction::All);
    _channel1Action.setWidgetFlags(ChannelAction::All);
    _channel2Action.setWidgetFlags(ChannelAction::All);
    _channel3Action.setWidgetFlags(ChannelAction::All);
    _colorSpaceAction.setWidgetFlags(OptionAction::All);
    _colorMapAction.setWidgetFlags(ColorMapAction::Settings | ColorMapAction::ResetPushButton);
    _interpolationTypeAction.setWidgetFlags(OptionAction::All);

    _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

    _channel1Action.getEnabledAction().setChecked(true);
    _channel1Action.getEnabledAction().setEnabled(false);
    _channel2Action.getEnabledAction().setEnabled(true);

    const auto dimensionNames = _layerAction.getLayer().getDimensionNames();

    _channel1Action.getDimensionAction().setOptions(dimensionNames);
    _channel2Action.getDimensionAction().setOptions(dimensionNames);
    _channel3Action.getDimensionAction().setOptions(dimensionNames);

    if (_layerAction.getLayer().getNumberOfImages() >= 2)
        _channel2Action.getDimensionAction().setCurrentIndex(1);

    if (_layerAction.getLayer().getNumberOfImages() >= 3)
        _channel3Action.getDimensionAction().setCurrentIndex(2);

    const auto updateChannelActions = [this]() -> void {
        const std::vector<bool> channelEnabled = {
            _channel1Action.getEnabledAction().isChecked(),
            _channel2Action.getEnabledAction().isChecked(),
            _channel3Action.getEnabledAction().isChecked()
        };

        const auto numberOfEnabledChannels = std::count_if(channelEnabled.begin(), channelEnabled.end(), [this](bool enabled) {
            return enabled;
        });

        switch (numberOfEnabledChannels)
        {
            case 1:
                _colorMapAction.setColorMapType(ColorMap::Type::OneDimensional);
                _colorMapAction.setEnabled(true);
                break;

            case 2:
                _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);
                _colorMapAction.setEnabled(true);
                break;

            case 3:
                _colorMapAction.setEnabled(false);
                break;

            default:
                break;
        }
        
        _channel3Action.getEnabledAction().setEnabled(channelEnabled[0] && channelEnabled[1]);

        const auto allChannelsAreEnabled = channelEnabled[0] && channelEnabled[1] && channelEnabled[2];

        _colorSpaceAction.setEnabled(allChannelsAreEnabled);

        if (allChannelsAreEnabled) {
            switch (static_cast<ColorSpace>(_colorSpaceAction.getCurrentIndex()))
            {
                case ColorSpace::RGB:
                    _channel1Action.setText("Red");
                    _channel2Action.setText("Green");
                    _channel3Action.setText("Blue");
                    break;

                case ColorSpace::HSL:
                    _channel1Action.setText("Hue");
                    _channel2Action.setText("Saturation");
                    _channel3Action.setText("Lightness");
                    break;

                case ColorSpace::LAB:
                    _channel1Action.setText("L");
                    _channel2Action.setText("A");
                    _channel3Action.setText("B");
                    break;

                default:
                    break;
            }
        }
        else {
            _channel1Action.setText("Channel 1");
            _channel2Action.setText("Channel 2");
            _channel3Action.setText("Channel 3");
        }
    };

    connect(&_channel1Action.getEnabledAction(), &ToggleAction::toggled, this, updateChannelActions);
    connect(&_channel2Action.getEnabledAction(), &ToggleAction::toggled, this, updateChannelActions);
    connect(&_channel3Action.getEnabledAction(), &ToggleAction::toggled, this, updateChannelActions);
    connect(&_colorSpaceAction, &OptionAction::currentIndexChanged, this, updateChannelActions);

    updateChannelActions();
}
