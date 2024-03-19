#pragma once

#include <actions/GroupsAction.h>

using namespace mv::gui;

/**
 * Edit layer action class
 *
 * Action class for editing an image layer
 *
 * @author Thomas Kroes
 */
class EditLayerAction : public GroupsAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE EditLayerAction(QObject* parent, const QString& title);
};

Q_DECLARE_METATYPE(EditLayerAction)

inline const auto editLayerActionMetaTypeId = qRegisterMetaType<EditLayerAction*>("EditLayerAction");
