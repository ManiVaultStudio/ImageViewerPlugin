#pragma once

#include "event/EventListener.h"

#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"
#include "actions/ColorMapAction.h"
#include "actions/ToggleAction.h"
#include "actions/ColorAction.h"

#include "ScalarChannelAction.h"

#include <QTimer>

class Layer;

using namespace hdps::gui;

/**
 * Image action class
 *
 * Action class for layer image settings
 *
 * @author Thomas Kroes
 */
class ImageAction : public GroupAction
{
Q_OBJECT

public:

    /**
     * Constructor
     * @param layer Reference to layer
     */
    ImageAction(Layer& layer);

    /** Perform action initialization */
    void init();

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

    /** Get the number of active scalar channels */
    const std::uint32_t getNumberOfActiveScalarChannels() const;

protected: // Color map

    /** Get color map image */
    QImage getColorMapImage() const;

    /** Update the color map image and notify others */
    void updateColorMapImage();

protected:

    /** Updates the scalar channel actions */
    void updateScalarChannelActions();

public: // Action getters

    DecimalAction& getOpacityAction() { return _opacityAction; }
    IntegralAction& getSubsampleFactorAction() { return _subsampleFactorAction; }
    OptionAction& getColorSpaceAction() { return _colorSpaceAction; }
    ScalarChannelAction& getScalarChannel1Action() { return _scalarChannel1Action; }
    ScalarChannelAction& getScalarChannel2Action() { return _scalarChannel2Action; }
    ScalarChannelAction& getScalarChannel3Action() { return _scalarChannel3Action; }
    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    OptionAction& getInterpolationTypeAction() { return _interpolationTypeAction; }
    ToggleAction& getUseConstantColorAction() { return _useConstantColorAction; }
    ColorAction& getConstantColorAction() { return _constantColorAction; }

signals:

    /** Signals the layer image changed */
    void changed();

    /**
     * Signals the scalar channel changed
     * @param scalarChannelAction Reference to scalar channel action that changed
     */
    void channelChanged(ScalarChannelAction& scalarChannelAction);

protected:
    Layer&                  _layer;                         /** Reference to layer */
    DecimalAction           _opacityAction;                 /** Opacity action */
    IntegralAction          _subsampleFactorAction;         /** Subsample factor action */
    OptionAction            _colorSpaceAction;              /** Color space action */
    ScalarChannelAction     _scalarChannel1Action;          /** Scalar channel 1 action */
    ScalarChannelAction     _scalarChannel2Action;          /** Scalar channel 2 action */
    ScalarChannelAction     _scalarChannel3Action;          /** Scalar channel 3 action */
    ColorMapAction          _colorMapAction;                /** Color map action */
    OptionAction            _interpolationTypeAction;       /** Interpolation type action */
    ToggleAction            _useConstantColorAction;        /** Constant color action */
    ColorAction             _constantColorAction;           /** Color action */
    hdps::EventListener     _eventListener;                 /** Listen to HDPS events */
    QTimer                  _updateSelectionTimer;          /** Timer to update layer selection when appropriate */
    QTimer                  _updateScalarDataTimer;         /** Timer to update layer scalar data when appropriate */

    static const std::int32_t LAZY_UPDATE_INTERVAL = 60;
};
