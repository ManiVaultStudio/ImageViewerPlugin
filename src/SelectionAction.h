#pragma once

#include "actions/GroupAction.h"
#include "actions/PixelSelectionAction.h"
#include "actions/TriggerAction.h"

#include "util/PixelSelectionTool.h"

class QWidget;

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
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     */
    SelectionAction(QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

public: /** Action getters */

protected:
    QWidget*                _targetWidget;                          /** Pointer to target widget */
    PixelSelectionTool&     _pixelSelectionTool;                    /** Reference to pixel selection tool */
    TriggerAction           _createSubsetFromSelectionAction;       /** Create subset from selection action */
};
