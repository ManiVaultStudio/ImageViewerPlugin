#pragma once

#include "actions/WidgetAction.h"
#include "actions/TriggerAction.h"
#include "actions/DecimalAction.h"
#include "actions/ToggleAction.h"

class ImageViewerWidget;

using namespace hdps::gui;

/**
 * Navigation action class
 *
 * Action class for navigation
 *
 * @author Thomas Kroes
 */
class NavigationAction : public WidgetAction
{
public:

    /** Widget class for pixel selection type action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param navigationAction Pointer to navigation action
         * @param state State of the widget
         */
        Widget(QWidget* parent, NavigationAction* navigationAction, const WidgetActionWidget::State& state);

    protected:
        friend class NavigationAction;
    };

protected:

    /**
     * Get widget representation of the navigation action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param imageViewerWidget Reference to image viewer widget
     */
    NavigationAction(ImageViewerWidget& imageViewerWidget);

    /** Determines whether the current value can be reset to its default */
    bool isResettable() const override {
        return false;
    };

    /** Reset the current value to the default value */
    void reset() override {};

    /** Get reference to image viewer widget */
    ImageViewerWidget& getImageViewerWidget();

public: // Action getters

    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    ToggleAction& getPanAction() { return _panAction; }

protected:
    ImageViewerWidget&  _imageViewerWidget;         /** Reference to image viewer widget */
    TriggerAction       _zoomOutAction;             /** Zoom out action */
    DecimalAction       _zoomPercentageAction;                /** Zoom action */
    TriggerAction       _zoomInAction;              /** Zoom in action */
    TriggerAction       _zoomExtentsAction;         /** Zoom extents action */
    ToggleAction        _panAction;                 /** Pan action */

    static const float zoomAmount;
};
