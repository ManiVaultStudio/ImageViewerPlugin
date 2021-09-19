#pragma once

#include "actions/Actions.h"

class QMenu;
class SettingsAction;

using namespace hdps::gui;

/**
 * Layers action class
 *
 * Action class for image layers display and interaction 
 *
 * @author Thomas Kroes
 */
class LayersAction : public WidgetAction
{
public:

    /** Widget class for layers action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param layersAction Pointer to layers action
         * @param state State of the widget
         */
        Widget(QWidget* parent, LayersAction* layersAction, const WidgetActionWidget::State& state);

    protected:
        TriggerAction   _removeLayerAction;     /** Remove layer action */
        TriggerAction   _moveLayerUpAction;     /** Move layer up action */
        TriggerAction   _moveLayerDownAction;   /** Move layer down action */

        friend class LayersAction;
    };

protected:

    /**
     * Get widget representation of the layers action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

public:

    /** 
     * Constructor
     * @param settingsAction Reference to settings action
     */
    LayersAction(SettingsAction& settingsAction);

public: // Action getters

    SettingsAction& getSettingsAction() { return _settingsAction; }
    GroupsAction& getCurrentLayerAction() { return _currentLayerAction; }

protected:
    SettingsAction&     _settingsAction;        /** Reference to settings action */
    GroupsAction        _currentLayerAction;    /** Current layer action */
};
