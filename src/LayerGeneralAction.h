#pragma once

#include "actions/GroupAction.h"
#include "actions/StringAction.h"

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

    StringAction& getNameAction() { return _nameAction; }

protected:
    LayerAction&    _layerAction;   /** Reference to layer action */
    StringAction    _nameAction;    /** Name action */
};
