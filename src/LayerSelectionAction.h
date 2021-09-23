#pragma once

#include "actions/GroupAction.h"
#include "actions/PixelSelectionAction.h"
#include "util/PixelSelectionTool.h"

class QWidget;

using namespace hdps::gui;

/**
 * Selection layer action class
 *
 * Action class for selection layer
 *
 * @author Thomas Kroes
 */
class LayerSelectionAction : public PixelSelectionAction
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
};
