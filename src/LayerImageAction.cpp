#include "LayerImageAction.h"
#include "LayerAction.h"
#include "Layer.h"

#include "util/ColorSpace.h"
#include "util/Interpolation.h"

using namespace hdps;
using namespace hdps::util;

LayerImageAction::LayerImageAction(LayerAction& layerAction) :
    GroupAction(&layerAction, true),
    EventListener(),
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
    _useConstantColorAction(this, "Use constant color", true, true),
    _constantColorAction(this, "Constant color", QColor(Qt::white), QColor(Qt::white))
{
    setText("Image");
    setEventCore(Application::core());

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
    _useConstantColorAction.setToolTip("Use constant color to shade the image");
    _constantColorAction.setToolTip("Constant color");

    _opacityAction.setSuffix("%");

    _opacityAction.setWidgetFlags(DecimalAction::All);
    _channel1Action.setWidgetFlags(ChannelAction::All);
    _channel2Action.setWidgetFlags(ChannelAction::All);
    _channel3Action.setWidgetFlags(ChannelAction::All);
    _colorSpaceAction.setWidgetFlags(OptionAction::All);
    _colorMapAction.setWidgetFlags(ColorMapAction::Settings | ColorMapAction::ResetPushButton);
    _interpolationTypeAction.setWidgetFlags(OptionAction::All);
    _useConstantColorAction.setWidgetFlags(ToggleAction::CheckBoxAndResetPushButton);
    _constantColorAction.setWidgetFlags(ColorAction::All);

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
        switch (static_cast<ColorSpaceType>(_colorSpaceAction.getCurrentIndex()))
        {
            case ColorSpaceType::Mono:
                _channel1Action.getEnabledAction().setChecked(true);
                _channel2Action.getEnabledAction().setChecked(false);
                _channel3Action.getEnabledAction().setChecked(false);
                _channel1Action.setText("Channel 1");
                _channel2Action.setText("Channel 2");
                _channel3Action.setText("Channel 3");
                _colorMapAction.setEnabled(true);
                _colorMapAction.setColorMapType(ColorMap::Type::OneDimensional);
                break;

            case ColorSpaceType::Duo:
                _channel1Action.getEnabledAction().setChecked(true);
                _channel2Action.getEnabledAction().setChecked(true);
                _channel3Action.getEnabledAction().setChecked(false);
                _channel1Action.setText("Channel 1");
                _channel2Action.setText("Channel 2");
                _channel3Action.setText("Channel 3");
                _colorMapAction.setEnabled(true);
                _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);
                break;

            case ColorSpaceType::RGB:
                _channel1Action.getEnabledAction().setChecked(true);
                _channel2Action.getEnabledAction().setChecked(true);
                _channel3Action.getEnabledAction().setChecked(true);
                _channel1Action.setText("Red");
                _channel2Action.setText("Green");
                _channel3Action.setText("Blue");
                _colorMapAction.setEnabled(false);
                break;

            case ColorSpaceType::HSL:
                _channel1Action.getEnabledAction().setChecked(true);
                _channel2Action.getEnabledAction().setChecked(true);
                _channel3Action.getEnabledAction().setChecked(true);
                _channel1Action.setText("Hue");
                _channel2Action.setText("Saturation");
                _channel3Action.setText("Lightness");
                _colorMapAction.setEnabled(false);
                break;

            case ColorSpaceType::LAB:
                _channel1Action.getEnabledAction().setChecked(true);
                _channel2Action.getEnabledAction().setChecked(true);
                _channel3Action.getEnabledAction().setChecked(true);
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

    const auto updateConstantColor = [this]() -> void {
        _constantColorAction.setEnabled(_useConstantColorAction.isChecked());
    };

    connect(&_useConstantColorAction, &ToggleAction::toggled, this, updateConstantColor);

    connect(&_channel1Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel1Action);
    });
    
    connect(&_channel2Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel2Action);
    });

    connect(&_channel3Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel3Action);
    });

    // Re-compute the selection channel when the selection changes
    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        if (dataEvent->getType() == hdps::EventType::SelectionChanged) {
            auto selectionChangedEvent = static_cast<hdps::SelectionChangedEvent*>(dataEvent);

            if (selectionChangedEvent->dataSetName != _layerAction.getLayer().getPoints()->getName())
                return;

            _channelSelectionAction.computeScalarData();
        }
    });

    updateChannelActions();
    updateConstantColor();
}

const std::uint32_t LayerImageAction::getNumberOfActiveChannels() const
{
    switch (static_cast<ColorSpaceType>(_colorSpaceAction.getCurrentIndex()))
    {
        case ColorSpaceType::Mono:
            return 1;

        case ColorSpaceType::Duo:
            return 2;

        case ColorSpaceType::RGB:
        case ColorSpaceType::HSL:
        case ColorSpaceType::LAB:
            return 3;

        default:
            break;
    }

    return 0;
}
