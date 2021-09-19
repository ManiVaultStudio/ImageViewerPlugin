#pragma once

#include "actions/GroupAction.h"

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
    LayerAction&    _layerAction;   /** Reference to layer action */
};
