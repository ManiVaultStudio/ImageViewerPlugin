#pragma once

#include "actions/WidgetAction.h"
#include "actions/DecimalAction.h"

class ChannelAction;

using namespace hdps::gui;

/**
 * Window/level settings action class
 *
 * Action class for window/level settings
 *
 * @author Thomas Kroes
 */
class WindowLevelAction : public WidgetAction
{
Q_OBJECT

public:

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param windowLevelAction Pointer to window level action
         * @param state State of the widget
         */
        Widget(QWidget* parent, WindowLevelAction* windowLevelAction, const WidgetActionWidget::State& state);

    protected:
        friend class WindowLevelAction;
    };

protected:

    /**
     * Get widget representation of the window level action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param channelAction Reference to channel action
     */
    WindowLevelAction(ChannelAction& channelAction);

public: /** Action getters */

    DecimalAction& getWindowAction() { return _windowAction; }
    DecimalAction& getLevelAction() { return _levelAction; }

signals:

    /** Signals the window/level changed */
    void changed(WindowLevelAction& windowLevelAction);

protected:
    ChannelAction&      _channelAction;     /** Reference to channel action */
    DecimalAction       _windowAction;      /** Window action */
    DecimalAction       _levelAction;       /** Level action */
};
