#pragma once

#include "actions/GroupAction.h"

class LayerAction;

using namespace hdps::gui;

/**
 * Common layer action class
 *
 * Action class for common layer settings
 *
 * @author Thomas Kroes
 */
class LayerCommonAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param layerAction Reference to layer action
     */
    LayerCommonAction(LayerAction& layerAction);

protected:
    LayerAction&    _layerAction;     /** Reference to layer action */
};
