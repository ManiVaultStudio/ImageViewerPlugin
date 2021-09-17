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
class CommonLayerAction : public WidgetAction
{
public:

    /** 
     * Constructor
     * @param layerAction Pointer to layer action
     */
    CommonLayerAction(LayerAction* layerAction);

protected:
    LayerAction*    _layerAction;     /** Pointer to layer action */
};
