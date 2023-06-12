#pragma once

#include <actions/GroupAction.h>
#include <actions/ToggleAction.h>
#include <actions/StringAction.h>
#include <actions/ColorAction.h>
#include <actions/DecimalAction.h>

#include "PositionAction.h"

class Layer;

using namespace hdps::gui;

/**
 * General action class
 *
 * Action class for general layer settings
 *
 * @author Thomas Kroes
 */
class GeneralAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param layer Reference to layer
     */
    GeneralAction(Layer& layer);

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

public: /** Action getters */

    StringAction& getDatasetNameAction() { return _datasetNameAction; }
    ToggleAction& getVisibleAction() { return _visibleAction; }
    ColorAction& getColorAction() { return _colorAction; }
    StringAction& getNameAction() { return _nameAction; }
    PositionAction& getPositionAction() { return _positionAction; }
    DecimalAction& getScaleAction() { return _scaleAction; }

protected:
    Layer&          _layer;                 /** Reference to layer */
    ToggleAction    _visibleAction;         /** Visible action */
    StringAction    _datasetNameAction;     /** Dataset name action */
    ColorAction     _colorAction;           /** Color action */
    StringAction    _nameAction;            /** Name action */
    PositionAction  _positionAction;        /** Position action */
    DecimalAction   _scaleAction;           /** Scale action */
};
