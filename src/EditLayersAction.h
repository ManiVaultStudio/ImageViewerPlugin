#pragma once

#include "actions/Actions.h"

#include <QRandomGenerator>

class QMenu;
class SettingsAction;

using namespace hdps::gui;

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

private:
    SettingsAction&     _settingsAction;            /** Reference to settings action */
    TriggerAction       _removeLayerAction;         /** Remove layer action */
    TriggerAction       _duplicateLayerAction;      /** Duplicate layer action */
    TriggerAction       _moveLayerToTopAction;      /** Move layer to top action */
    TriggerAction       _moveLayerUpAction;         /** Move layer up action */
    TriggerAction       _moveLayerDownAction;       /** Move layer down action */
    TriggerAction       _moveLayerToBottomAction;   /** Move layer to bottom action */

    static QRandomGenerator     rng;    /** Random number generator for pseudo-random layer colors */
};
