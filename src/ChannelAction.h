#pragma once

#include "actions/WidgetAction.h"
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

    OptionAction& getDimensionAction() { return _dimensionAction; }

protected:
    LayerImageAction&       _layerImageAction;      /** Reference to layer image action */
    OptionAction            _dimensionAction;       /** Selected dimension action */
};
