#include "ImageAction.h"
#include "Layer.h"

#include "util/ColorSpace.h"
#include "util/Interpolation.h"

using namespace hdps;
using namespace hdps::util;

ImageAction::ImageAction(Layer& layer) :
    GroupAction(&layer, true),
    EventListener(),
    _layer(layer),
    _opacityAction(this, "Opacity", 0.0f, 100.0f, 100.0f, 100.0f, 1),
    _subsampleFactorAction(this, "Subsample", 1, 8, 1, 1),
    _colorSpaceAction(this, "Color space", colorSpaces.values(), "Mono", "Mono"),
    _channel1Action(*this, ChannelAction::Channel1, ChannelAction::channelIndexes.value(ChannelAction::Channel1)),
    _channel2Action(*this, ChannelAction::Channel2, ChannelAction::channelIndexes.value(ChannelAction::Channel2)),
    _channel3Action(*this, ChannelAction::Channel3, ChannelAction::channelIndexes.value(ChannelAction::Channel3)),
    _channelMaskAction(*this, ChannelAction::Mask, ChannelAction::channelIndexes.value(ChannelAction::Mask)),
    _channelSelectionAction(*this, ChannelAction::Selection, ChannelAction::channelIndexes.value(ChannelAction::Selection)),
    _colorMapAction(this, "Color map", ColorMap::Type::OneDimensional, "Black to white", "Black to white"),
    _interpolationTypeAction(this, "Interpolate", interpolationTypes.values(), "Bilinear", "Bilinear"),
    _useConstantColorAction(this, "Use constant color", false, false),
    _constantColorAction(this, "Constant color", QColor(Qt::white), QColor(Qt::white))
{
    setText("Image");
    setEventCore(Application::core());

    _channelMaskAction.setVisible(false);
    _channelSelectionAction.setVisible(false);

    _opacityAction.setToolTip("Image layer opacity");
    _subsampleFactorAction.setToolTip("Subsampling factor");
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

    _colorMapAction.setColorMapType(ColorMap::Type::TwoDimensional);

    // Get the dimension names of the points dataset
    const auto dimensionNames = _layer.getDimensionNames();

    // Set color space to mono in case of one dimension
    if (dimensionNames.count() == 1)
        _colorSpaceAction.setCurrentIndex(0);
    
    // Set color space to duo in case of two dimensions
    if (dimensionNames.count() == 2)
        _colorSpaceAction.setCurrentIndex(1);

    // Set color space to rgb in case of three (or more) dimensions
    if (dimensionNames.count() >= 3)
        _colorSpaceAction.setCurrentIndex(0);

    // Set channel dimension names
    _channel1Action.getDimensionAction().setOptions(dimensionNames);
    _channel2Action.getDimensionAction().setOptions(dimensionNames);
    _channel3Action.getDimensionAction().setOptions(dimensionNames);

    _channel1Action.getDimensionAction().setCurrentIndex(0);
    _channel1Action.getDimensionAction().setDefaultIndex(0);

    if (_layer.getNumberOfImages() >= 2) {
        _channel2Action.getDimensionAction().setCurrentIndex(1);
        _channel2Action.getDimensionAction().setDefaultIndex(1);
    }

    if (_layer.getNumberOfImages() >= 3) {
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

    const auto useConstantColorToggled = [this]() {
        _constantColorAction.setEnabled(_useConstantColorAction.isChecked());
        _layer.invalidate();
    };

    connect(&_useConstantColorAction, &ToggleAction::toggled, this, useConstantColorToggled);

    connect(&_channel1Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel1Action);
    });
    
    connect(&_channel2Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel2Action);
    });

    connect(&_channel3Action, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channel3Action);
    });

    connect(&_channelSelectionAction, &ChannelAction::changed, this, [this]() {
        emit channelChanged(_channelSelectionAction);
    });

    const auto render = [this]() {
        _layer.invalidate();
    };

    connect(&_opacityAction, &DecimalAction::valueChanged, this, render);
    connect(&_subsampleFactorAction, &IntegralAction::valueChanged, this, render);
    connect(&_interpolationTypeAction, &OptionAction::currentIndexChanged, this, render);
    connect(&_constantColorAction, &ColorAction::colorChanged, this, render);

    updateChannelActions();
    useConstantColorToggled();

    // Register for events for images datasets
    registerDataEventByType(ImageType, [this](DataEvent* dataEvent) {

        switch (dataEvent->getType())
        {
        case EventType::DataAboutToBeRemoved:
        {
            Application::core()->unregisterEventListener(this);
            break;
        }

        default:
            break;
        }
    });

    // Register for events for points datasets
    registerDataEventByType(PointType, [this](DataEvent* dataEvent) {

        // The points dataset might have been deleted so check first if it is valid
        if (!_layer.getPoints().isValid())
            return;

        // Only process points dataset that is referenced by us
        if (dataEvent->dataSetName != _layer.getPoints()->getName())
            return;

        switch (dataEvent->getType())
        {
            case EventType::DataChanged:
            {
                _channel1Action.computeScalarData();
                _channel2Action.computeScalarData();
                _channel3Action.computeScalarData();

                break;
            }

            case EventType::SelectionChanged:
            {
                _layer.computeSelectionIndices();
                break;
            }

            default:
                break;
        }
    });
}

const std::uint32_t ImageAction::getNumberOfActiveChannels() const
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
