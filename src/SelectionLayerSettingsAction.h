#pragma once

#include "actions/GroupAction.h"

#include "util/DatasetRef.h"
#include "ImageData/Images.h"

#include "LayerAction.h"

using namespace hdps::util;
using namespace hdps::gui;

class SelectionLayerAction;

/**
 * Selection layer settings action class
 *
 * Action class for selection layer settings
 *
 * @author Thomas Kroes
 */
class SelectionLayerSettingsAction : public GroupAction
{
public:

    /** 
     * Constructor
     * @param selectionLayerAction Reference to selection layer action
     */
    SelectionLayerSettingsAction(SelectionLayerAction& selectionLayerAction);

protected:
    SelectionLayerAction&   _selectionLayerAction;          /** Reference to selection layer action */
    DatasetRef<Images>      _images;                        /** Reference to images */
    OptionAction            _selectionTypeAction;           /** Selection type action */
    OptionAction            _selectionModifierAction;       /** Selection modifier action */
    DecimalAction           _brushRadiusAction;             /** Brush radius action */
    ColorAction             _overlayColorAction;            /** Overlay color action */
};
