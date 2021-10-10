#pragma once

#include "actions/GroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/StringAction.h"
#include "actions/TriggerAction.h"

class Layer;

using namespace hdps::gui;

/**
 * Subset action class
 *
 * Action class for creating images/points subsets
 *
 * @author Thomas Kroes
 */
class SubsetAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param layer Reference to layer
     */
    SubsetAction(Layer& layer);

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

public: /** Action getters */

    ToggleAction& getVisibleAction() { return _fromRegionAction; }
    StringAction& getNameAction() { return _nameAction; }
    TriggerAction& getCreateAction() { return _createAction; }

protected:
    Layer&          _layer;                 /** Reference to layer */
    ToggleAction    _fromRegionAction;      /** Whether to create an image set from the selected region of pixels */
    StringAction    _nameAction;            /** Subset name action */
    TriggerAction   _createAction;          /** Create the subset action */
};
