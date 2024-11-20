#pragma once

#include <actions/GroupAction.h>
#include <actions/DecimalAction.h>

using namespace mv::gui;

class GeneralAction;

/**
 * Position action class
 *
 * Action class for configuring layer position
 *
 * @author Thomas Kroes
 */
class PositionAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE PositionAction(QObject* parent, const QString& title);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const mv::VariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    mv::VariantMap toVariantMap() const override;

signals:

    /** Signals that the position changed */
    void changed();

public: /** Action getters */

    DecimalAction& getXAction() { return _xAction; }
    DecimalAction& getYAction() { return _yAction; }

protected:
    DecimalAction   _xAction;   /** X-position action */
    DecimalAction   _yAction;   /** Y-position action */
};

Q_DECLARE_METATYPE(PositionAction)

inline const auto positionActionMetaTypeId = qRegisterMetaType<PositionAction*>("PositionAction");
