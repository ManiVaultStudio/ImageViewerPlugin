#pragma once

#include "actions/Actions.h"

#include "LayerCommonAction.h"
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

protected:
    Layer&                  _layer;             /** Reference to layer */
    LayerCommonAction       _commonAction;      /** Common action */
    LayerImageAction        _imageAction;       /** Image action */
    LayerSelectionAction    _selectionAction;   /** Selection action */
};
