#pragma once

#include <actions/TriggerAction.h>
#include <actions/GroupAction.h>

#include <widgets/HierarchyWidget.h>

#include <QRandomGenerator>

class QMenu;
class SettingsAction;

using namespace mv::gui;

/**
 * Edit layers action class
 *
 * Action class for managing image layers
 *
 * @author Thomas Kroes
 */
class EditLayersAction : public WidgetAction
{
public:

    /** Widget class for layers action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param editLayersAction Pointer to edit layers action
         */
        Widget(QWidget* parent, EditLayersAction* editLayersAction);

    private:
        HierarchyWidget     _hierarchyWidget;       /** Layers widget */

        friend class EditLayersAction;
    };

protected:

    /**
     * Get widget representation of the layers action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE EditLayersAction(QObject* parent, const QString& title);

    /** Get pseudo-random layer color */
    static QColor getRandomLayerColor();

public: // Action getters

    SettingsAction& getSettingsAction() { return _settingsAction; }
    TriggerAction& getRemoveLayerAction() { return _removeLayerAction; }
    TriggerAction& getMoveLayerToTopAction() { return _moveLayerToTopAction; }
    TriggerAction& getMoveLayerUpAction() { return _moveLayerUpAction; }
    TriggerAction& getMoveLayerDownAction() { return _moveLayerDownAction; }
    TriggerAction& getMoveLayerToBottomAction() { return _moveLayerToBottomAction; }
    GroupAction& getOperationsAction() { return _operationsAction; }

private:
    SettingsAction&     _settingsAction;            /** Reference to settings action */
    TriggerAction       _removeLayerAction;         /** Remove layer action */
    TriggerAction       _moveLayerToTopAction;      /** Move layer to top action */
    TriggerAction       _moveLayerUpAction;         /** Move layer up action */
    TriggerAction       _moveLayerDownAction;       /** Move layer down action */
    TriggerAction       _moveLayerToBottomAction;   /** Move layer to bottom action */
    GroupAction         _operationsAction;          /** Action for grouping various operations */

    static QRandomGenerator     rng;    /** Random number generator for pseudo-random layer colors */
};
