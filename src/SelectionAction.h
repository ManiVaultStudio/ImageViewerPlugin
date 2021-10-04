#pragma once

#include "actions/ToggleAction.h"
#include "actions/GroupAction.h"
#include "actions/PixelSelectionAction.h"
#include "actions/TriggerAction.h"

#include "util/PixelSelectionTool.h"

class QWidget;

class LayerAction;

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
     * @param layerAction Reference to layer action
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     */
    SelectionAction(LayerAction& layerAction, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

    /** Get selection boundaries */
    QRect getSelectionBoundaries() const;

public: /** Action getters */

    ToggleAction& getShowRegionAction() { return _showRegionAction; }
    GroupAction& getGroupAction() { return _groupAction; }

protected:
    LayerAction&            _layerAction;           /** Reference to layer action */
    QWidget*                _targetWidget;          /** Pointer to target widget */
    PixelSelectionTool&     _pixelSelectionTool;    /** Reference to pixel selection tool */
    ToggleAction            _showRegionAction;      /** Show region action */
    GroupAction             _groupAction;           /** Group action */
};
