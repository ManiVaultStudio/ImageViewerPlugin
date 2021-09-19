#pragma once

#include "actions/GroupAction.h"

class LayerAction;

using namespace hdps::gui;

/**
 * Selection layer action class
 *
 * Action class for selection layer
 *
 * @author Thomas Kroes
 */
class LayerSelectionAction : public GroupAction
{
public:

    /**
     * Constructor
     * @param layerAction Reference to layer action
     */
    LayerSelectionAction(LayerAction& layerAction);

public: /** Action getters */

protected:
    LayerAction&    _layerAction;       /** Reference to layer action */
    QImage          _image;             /** Selection image */
};
