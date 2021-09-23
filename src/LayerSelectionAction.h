#pragma once

#include "actions/Actions.h"
#include "util/PixelSelectionTool.h"

#include "PixelSelectionAction.h"

class QWidget;

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
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     */
    LayerSelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

public: /** Action getters */

protected:
    QWidget*                _targetWidget;              /** Pointer to target widget */
    PixelSelectionTool&     _pixelSelectionTool;        /** Reference to pixel selection tool */
    PixelSelectionAction    _pixelSelectionAction;      /** Pixel selection action */
};
