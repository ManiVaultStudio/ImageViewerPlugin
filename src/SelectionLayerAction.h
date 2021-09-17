#pragma once

#include "util/DatasetRef.h"
#include "ImageData/Images.h"

#include "LayerAction.h"

#include "CommonLayerAction.h"
#include "SelectionLayerSettingsAction.h"

class LayersAction;

using namespace hdps::util;
using namespace hdps::gui;

/**
 * Selection layer action class
 *
 * Action class for selection layer
 *
 * @author Thomas Kroes
 */
class SelectionLayerAction : public LayerAction
{
public:

    /** 
     * Constructor
     * @param layersAction Reference to layers action
     */
    SelectionLayerAction(LayersAction& layersAction);

public: /** Action getters */

    CommonLayerAction& getCommonLayerAction() { return _commonLayerAction; }
    SelectionLayerSettingsAction& getSelectionLayerSettingsAction() { return _selectionLayerSettingsAction; }

protected:
    LayersAction&                   _layersAction;                      /** Reference to layers action */
    CommonLayerAction               _commonLayerAction;                 /** Common layer action */
    SelectionLayerSettingsAction    _selectionLayerSettingsAction;      /** Selection layer settings action */
    QImage                          _image;                             /** Selection image */
};
