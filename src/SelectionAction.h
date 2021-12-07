#pragma once

#include "actions/ToggleAction.h"
#include "actions/GroupAction.h"
#include "actions/PixelSelectionAction.h"
#include "actions/TriggerAction.h"

#include "util/PixelSelectionTool.h"

class QWidget;

class Layer;

using namespace hdps::util;
using namespace hdps::gui;

/**
 * Selection action class
 *
 * Action class for selection
 *
 * @author Thomas Kroes
 */
class SelectionAction : public PixelSelectionAction
{
public:

    /**
     * Constructor
     * @param layer Reference to layer
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     */
    SelectionAction(Layer& layer, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

    /** Get reference to parent layer */
    Layer& getLayer() { return _layer; }

    /** Get selection rectangle in image coordinates */
    QRect getImageSelectionRectangle() const;

public: /** Action getters */

    ToggleAction& getShowRegionAction() { return _showRegionAction; }
    ToggleAction& getSelectPixelsInViewAction() { return _selectPixelsInViewAction; }
    GroupAction& getGroupAction() { return _groupAction; }

protected:
    Layer&                  _layer;                     /** Reference to layer */
    QWidget*                _targetWidget;              /** Pointer to target widget */
    PixelSelectionTool&     _pixelSelectionTool;        /** Reference to pixel selection tool */
    ToggleAction            _showRegionAction;          /** Show region action */
    ToggleAction            _selectPixelsInViewAction;  /** Select pixels in view action */
    GroupAction             _groupAction;               /** Group action */
};
