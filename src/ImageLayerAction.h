#pragma once

#include "LayerAction.h"

#include "CommonLayerAction.h"

class LayersAction;

using namespace hdps::gui;

/**
 * Image layer action class
 *
 * Action class for image layer settings
 *
 * @author Thomas Kroes
 */
class ImageLayerAction : public LayerAction
{
public:

    /**
     * Constructor
     * @param layersAction Reference to layers action
     */
    ImageLayerAction(LayersAction& layersAction);

public: /** Action getters */

    CommonLayerAction& getCommonLayerAction() { return _commonLayerAction; }

protected:
    LayersAction&       _layersAction;          /** Reference to layers action */
    CommonLayerAction   _commonLayerAction;     /** Common layer action */
};
