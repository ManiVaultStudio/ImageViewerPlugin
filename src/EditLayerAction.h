#pragma once

#include <actions/GroupAction.h>

using namespace hdps::gui;

/**
 * Edit layer action class
 *
 * Action class for editing an image layer
 *
 * @author Thomas Kroes
 */
class EditLayerAction : public GroupAction
{
public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE EditLayerAction(QObject* parent, const QString& title);

public: // Action getters

protected:
};
