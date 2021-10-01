#pragma once

#include "actions/Actions.h"

#include "GeneralAction.h"
#include "ImageAction.h"
#include "SelectionAction.h"

using namespace hdps::gui;

class Layer;
class LayersAction;

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
    LayerAction(Layer& layer, LayersAction& layersAction);

    /** Get reference to layer */
    Layer& getLayer();

public: /** Action getters */

    LayersAction& getLayersAction() { return _layersAction; }
    GeneralAction& getGeneralAction() { return _generalAction; }
    ImageAction& getImageAction() { return _imageAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }

protected:
    Layer&              _layer;                 /** Reference to layer */
    LayersAction&       _layersAction;          /** Reference to layers action */
    GeneralAction       _generalAction;         /** General action */
    ImageAction         _imageAction;           /** Image action */
    SelectionAction     _selectionAction;       /** Selection action */
};
