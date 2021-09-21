#pragma once

#include "actions/Actions.h"

#include "LayerGeneralAction.h"
#include "LayerImageAction.h"
#include "LayerSelectionAction.h"

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

    LayerGeneralAction& getGeneralAction() { return _generalAction; }
    LayerImageAction& getImageAction() { return _imageAction; }
    LayerSelectionAction& getSelectionAction() { return _selectionAction; }

protected:
    Layer&                  _layer;             /** Reference to layer */
    LayerGeneralAction      _generalAction;     /** General action */
    LayerImageAction        _imageAction;       /** Image action */
    LayerSelectionAction    _selectionAction;   /** Selection action */
};
