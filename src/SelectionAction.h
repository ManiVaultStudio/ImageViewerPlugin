#pragma once

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

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param selectionAction Pointer to selection action
         * @param state State of the widget
         */
        Widget(QWidget* parent, SelectionAction* selectionAction, const WidgetActionWidget::State& state);

    protected:
        friend class SelectionAction;
    };

protected:

    /**
     * Get widget representation of the selection action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /**
     * Constructor
     * @param layerAction Reference to layer action
     * @param targetWidget Pointer to target widget
     * @param pixelSelectionTool Reference to pixel selection tool
     */
    SelectionAction(LayerAction& layerAction, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool);

public: /** Action getters */

    GroupAction& getGroupAction() { return _groupAction; }

protected:
    LayerAction&            _layerAction;           /** Reference to layer action */
    QWidget*                _targetWidget;          /** Pointer to target widget */
    PixelSelectionTool&     _pixelSelectionTool;    /** Reference to pixel selection tool */
    GroupAction             _groupAction;           /** Create subset from selection action */
};
