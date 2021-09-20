#pragma once

#include "actions/GroupAction.h"
#include "actions/StringAction.h"
#include "actions/DecimalAction.h"

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
    DecimalAction& getScaleAction() { return _scaleAction; }

protected:
    LayerAction&        _layerAction;       /** Reference to layer action */
    StringAction        _nameAction;        /** Name action */
    DecimalAction       _scaleAction;       /** Scale action */
};
