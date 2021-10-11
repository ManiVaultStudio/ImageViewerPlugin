#pragma once

#include "event/EventListener.h"

#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"
#include "actions/ColorMapAction.h"
#include "actions/ToggleAction.h"
#include "actions/ColorAction.h"

#include "ChannelAction.h"

class Layer;

using namespace hdps::gui;

/**
 * Image action class
 *
 * Action class for layer image settings
 *
 * @author Thomas Kroes
 */
class ImageAction : public GroupAction, public hdps::EventListener
{
Q_OBJECT

public:

    /**
     * Constructor
     * @param layer Reference to layer
     */
    ImageAction(Layer& layer);

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

    /** Get the number of active channels */
    const std::uint32_t getNumberOfActiveChannels() const;

public: // Action getters

    DecimalAction& getOpacityAction() { return _opacityAction; }
    IntegralAction& getSubsampleFactorAction() { return _subsampleFactorAction; }
    OptionAction& getColorSpaceAction() { return _colorSpaceAction; }
    ChannelAction& getChannel1Action() { return _channel1Action; }
    ChannelAction& getChannel2Action() { return _channel2Action; }
    ChannelAction& getChannel3Action() { return _channel3Action; }
    ChannelAction& getChannelMaskAction() { return _channelMaskAction; }
    ChannelAction& getChannelSelectionAction() { return _channelSelectionAction; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    OptionAction& getInterpolationTypeAction() { return _interpolationTypeAction; }
    ToggleAction& getUseConstantColorAction() { return _useConstantColorAction; }
    ColorAction& getConstantColorAction() { return _constantColorAction; }

signals:

    /** Signals the layer image changed */
    void changed();

    /**
     * Signals the channel changed
     * @param channelAction Reference to channel action
     */
    void channelChanged(ChannelAction& channelAction);

protected:
    Layer&              _layer;                         /** Reference to layer */
    DecimalAction       _opacityAction;                 /** Opacity action */
    IntegralAction      _subsampleFactorAction;         /** Subsample factor action */
    OptionAction        _colorSpaceAction;              /** Color space action */
    ChannelAction       _channel1Action;                /** Channel 1 action */
    ChannelAction       _channel2Action;                /** Channel 2 action */
    ChannelAction       _channel3Action;                /** Channel 3 action */
    ChannelAction       _channelMaskAction;             /** Mask channel action */
    ChannelAction       _channelSelectionAction;        /** Selection channel action */
    ColorMapAction      _colorMapAction;                /** Color map action */
    OptionAction        _interpolationTypeAction;       /** Interpolation type action */
    ToggleAction        _useConstantColorAction;        /** Constant color action */
    ColorAction         _constantColorAction;           /** Color action */
};
