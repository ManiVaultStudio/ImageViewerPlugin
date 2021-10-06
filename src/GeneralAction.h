#pragma once

#include "actions/Actions.h"

#include "ZoomAction.h"

class LayerAction;

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
     * @param layerAction Reference to layer action
     */
    GeneralAction(LayerAction& layerAction);

public: /** Action getters */

    LayerAction& getLayerAction() { return _layerAction; }
    ToggleAction& getVisibleAction() { return _visibleAction; }
    ColorAction& getColorAction() { return _colorAction; }
    StringAction& getNameAction() { return _nameAction; }
    DecimalAction& getScaleAction() { return _scaleAction; }
    DecimalAction& getXPositionAction() { return _xPositionAction; }
    DecimalAction& getYPositionAction() { return _yPositionAction; }
    ZoomAction& getZoomAction() { return _zoomAction; }

protected:
    LayerAction&    _layerAction;           /** Reference to layer action */
    ToggleAction    _visibleAction;         /** Visible action */
    ColorAction     _colorAction;           /** Color action */
    StringAction    _nameAction;            /** Name action */
    DecimalAction   _scaleAction;           /** Scale action */
    DecimalAction   _xPositionAction;       /** X-position action */
    DecimalAction   _yPositionAction;       /** Y-position action */
    ZoomAction      _zoomAction;            /** Zoom to extents action */
};
