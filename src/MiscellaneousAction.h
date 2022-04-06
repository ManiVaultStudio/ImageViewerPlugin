#pragma once

#include <actions/GroupAction.h>
#include <actions/IntegralRectangleAction.h>
#include <actions/DecimalRectangleAction.h>

class Layer;

using namespace hdps::gui;

/**
 * Miscellaneous action class
 *
 * Group action class for various actions
 *
 * @author Thomas Kroes
 */
class MiscellaneousAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param layer Reference to layer
     */
    MiscellaneousAction(Layer& layer);

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

public: // Action getters

    IntegralRectangleAction& getRoiLayerAction() { return _roiLayerAction; }
    DecimalRectangleAction& getRoiViewAction() { return _roiViewAction; }

protected:
    Layer&                      _layer;             /** Reference to layer */
    IntegralRectangleAction     _roiLayerAction;    /** Layer region of interest action */
    DecimalRectangleAction      _roiViewAction;     /** View region of interest action */
};
