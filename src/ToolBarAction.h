#pragma once

#include "actions/WidgetAction.h"
#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Toolbar action class
 *
 * Action class for toolbar
 *
 * @author Thomas Kroes
 */
class ToolBarAction : public WidgetAction
{
public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox        = 0x00001,      /** The widget includes a combobox */
        PushButtonGroup = 0x00002,      /** The widget includes push buttons in a group */
        ResetPushButton = 0x00004,      /** The widget includes a reset push button */

        Basic   = ComboBox,
        All     = ComboBox | PushButtonGroup | ResetPushButton
    };

public:

    /** Widget class for pixel selection type action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toolBarAction Pointer to toolbar action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ToolBarAction* toolBarAction, const WidgetActionWidget::State& state);

    protected:
        friend class ToolBarAction;
    };

protected:

    /**
     * Get widget representation of the pixel selection type action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    ToolBarAction(ImageViewerPlugin& imageViewerPlugin);

    /** Determines whether the current value can be reset to its default */
    bool isResettable() const override {
        return false;
    };

    /** Reset the current value to the default value */
    void reset() override {};

public: // Action getters

    ToggleAction& getPanAction() { return _panAction; }
    ToggleAction& getZoomAction() { return _zoomAction; }
    TriggerAction& getZoomAreaAction() { return _zoomAreaAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;         /** Reference to image viewer plugin */
    ToggleAction            _panAction;                 /** Pan action */
    ToggleAction            _zoomAction;                /** Zoom action */
    TriggerAction           _zoomAreaAction;            /** Zoom area action */
    TriggerAction           _zoomExtentsAction;         /** Zoom extents action */
};
