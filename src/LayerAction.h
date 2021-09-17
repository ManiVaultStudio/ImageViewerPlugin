#pragma once

#include "actions/Actions.h"

using namespace hdps::gui;

class LayersAction;

/**
 * Layer action class
 *
 * Base action class for layers
 *
 * @author Thomas Kroes
 */
class LayerAction : public WidgetAction
{
public:

    /** 
     * Constructor
     * @param layersAction Pointer to layers action
     */
    LayerAction(LayersAction* layersAction);

protected:
    LayersAction*   _layersAction;     /** Pointer to layers action */
};
