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
     * Get widget representation of the channel action
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

    ToggleAction& getPanAction() { return _panAction; }
    ToggleAction& getSelectAction() { return _selectAction; }
    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }

protected:
    ImageViewerWidget&  _imageViewerWidget;             /** Reference to image viewer widget */
    TriggerAction       _zoomOutAction;                 /** Zoom out action */
    DecimalAction       _zoomPercentageAction;          /** Zoom action */
    TriggerAction       _zoomInAction;                  /** Zoom in action */
    TriggerAction       _zoomExtentsAction;             /** Zoom extents action */
    ToggleAction        _panAction;                     /** Pan interaction mode action */
    ToggleAction        _selectAction;                  /** Select interaction mode action */
    QActionGroup        _interactionModeActionGroup;    /** Interaction mode action group */

    static const float zoomDeltaPercentage;
};
