#pragma once

#include "actions/Actions.h"

class LayerAction;

using namespace hdps::gui;

/**
 * Common layer action class
 *
 * Action class for common layer settings
 *
 * @author Thomas Kroes
 */
class LayerGeneralAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param layerAction Reference to layer action
     */
    LayerGeneralAction(LayerAction& layerAction);

public: /** Action getters */

    ToggleAction& getVisibleAction() { return _visibleAction; }
    ColorAction& getColorAction() { return _colorAction; }
    StringAction& getNameAction() { return _nameAction; }
    DecimalAction& getScaleAction() { return _scaleAction; }
    TriggerAction& getZoomToExtentsAction() { return _zoomToExtentsAction; }

protected:
    LayerAction&    _layerAction;               /** Reference to layer action */
    ToggleAction    _visibleAction;             /** Visible action */
    ColorAction     _colorAction;               /** Color action */
    StringAction    _nameAction;                /** Name action */
    DecimalAction   _scaleAction;               /** Scale action */
    TriggerAction   _zoomToExtentsAction;       /** Zoom to extents action */
};
