#pragma once

#include "actions/Actions.h"

#include "ZoomAction.h"

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
    DecimalAction& getScaleAction() { return _scaleAction; }
    DecimalAction& getXPositionAction() { return _xPositionAction; }
    DecimalAction& getYPositionAction() { return _yPositionAction; }
    ZoomAction& getZoomAction() { return _zoomAction; }

protected:
    Layer&          _layer;                 /** Reference to layer */
    ToggleAction    _visibleAction;         /** Visible action */
    StringAction    _datasetNameAction;     /** Dataset name action */
    ColorAction     _colorAction;           /** Color action */
    StringAction    _nameAction;            /** Name action */
    DecimalAction   _scaleAction;           /** Scale action */
    DecimalAction   _xPositionAction;       /** X-position action */
    DecimalAction   _yPositionAction;       /** Y-position action */
    ZoomAction      _zoomAction;            /** Zoom to extents action */
};
