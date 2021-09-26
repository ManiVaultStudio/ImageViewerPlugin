#pragma once

#include "actions/Actions.h"

#include "GeneralAction.h"
#include "ImageAction.h"

using namespace hdps::gui;

class Layer;

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
     * @param layer Reference to layer
     */
    LayerAction(Layer& layer);

    /** Get reference to layer */
    Layer& getLayer();

public: /** Action getters */

    GeneralAction& getGeneralAction() { return _generalAction; }
    ImageAction& getImageAction() { return _imageAction; }

protected:
    Layer&          _layer;             /** Reference to layer */
    GeneralAction   _generalAction;     /** General action */
    ImageAction     _imageAction;       /** Image action */
    
};
