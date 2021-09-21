#pragma once

#include "WindowLevelAction.h"

#include "actions/WidgetAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"

using namespace hdps::gui;

class LayerImageAction;

/**
 * Channel action class
 *
 * Layer channel class
 *
 * @author Thomas Kroes
 */
class ChannelAction : public WidgetAction
{
public:

    /** Describes the widget flags */
    enum WidgetFlag {
        CheckBox            = 0x00001,  /** The widget includes an enabled check box */
        ComboBox            = 0x00002,  /** The widget includes a dimension selection combobox */
        WindowLevelWidget   = 0x00004,  /** The widget includes a window/level settings widget */
        ResetPushButton     = 0x00008,  /** The widget includes a reset push button */

        All = CheckBox | ComboBox | WindowLevelWidget | ResetPushButton
    };

public:

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param channelAction Pointer to channel action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state);

    protected:
        friend class ChannelAction;
    };

protected:

    /**
     * Get widget representation of the settings action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param layerImageAction Reference to layer image action
     * @param name Name of the channel
     */
    ChannelAction(LayerImageAction& layerImageAction, const QString& name);

public: /** Action getters */

    ToggleAction& getEnabledAction() { return _enabledAction; }
    OptionAction& getDimensionAction() { return _dimensionAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

protected:
    LayerImageAction&   _layerImageAction;      /** Reference to layer image action */
    ToggleAction        _enabledAction;         /** Enabled action */
    OptionAction        _dimensionAction;       /** Selected dimension action */
    WindowLevelAction   _windowLevelAction;     /** Window/level action */
};
