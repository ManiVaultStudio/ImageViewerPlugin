#pragma once

#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"
#include "actions/ColorMapAction.h"
#include "actions/ToggleAction.h"

#include "ChannelAction.h"

class LayerAction;

using namespace hdps::gui;

/**
 * Image layer action class
 *
 * Action class for image layer settings
 *
 * @author Thomas Kroes
 */
class LayerImageAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param layerAction Reference to layer action
     */
    LayerImageAction(LayerAction& layerAction);

public: /** Action getters */

protected:
    LayerAction&        _layerAction;                   /** Reference to layer action */
    DecimalAction       _opacityAction;                 /** Opacity action */
    ChannelAction       _channel1Action;                /** Channel 1 action */
    ChannelAction       _channel2Action;                /** Channel 2 action */
    ChannelAction       _channel3Action;                /** Channel 3 action */
    OptionAction        _colorSpaceAction;              /** Color space action */
    ColorMapAction      _colorMapAction;                /** Color map action */
    OptionAction        _interpolationTypeAction;       /** Interpolation type action */
    ToggleAction        _constantColorAction;           /** Constant color action */
};
