#pragma once

#include "actions/GroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

#include "event/EventListener.h"

class LayerAction;

using namespace hdps::gui;

/**
 * Subset action class
 *
 * Action class for creating images/points subsets
 *
 * @author Thomas Kroes
 */
class SubsetAction : public GroupAction, public hdps::EventListener
{
public:

    /** 
     * Constructor
     * @param layerAction Reference to layer action
     */
    SubsetAction(LayerAction& layerAction);

public: /** Action getters */

    ToggleAction& getVisibleAction() { return _fromRegionAction; }
    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getCreateAction() { return _createAction; }

protected:
    LayerAction&    _layerAction;           /** Reference to layer action */
    ToggleAction    _fromRegionAction;      /** Whether to create an image set from the selected region of pixels */
    StringAction    _nameAction;            /** Subset name action */
    TriggerAction   _createAction;          /** Create the subset action */
};
