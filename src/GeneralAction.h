#pragma once

#include <actions/GroupAction.h>
#include <actions/ToggleAction.h>
#include <actions/StringAction.h>
#include <actions/ColorAction.h>
#include <actions/DecimalAction.h>

#include "PositionAction.h"

class Layer;

using namespace mv::gui;

/**
 * General action class
 *
 * Action class for general layer settings
 *
 * @author Thomas Kroes
 */
class GeneralAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE GeneralAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p layer
     * @param layer Pointer to owning layer
     */
    void initialize(Layer* layer);

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
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: /** Action getters */

    StringAction& getDatasetNameAction() { return _datasetNameAction; }
    ToggleAction& getVisibleAction() { return _visibleAction; }
    ColorAction& getColorAction() { return _colorAction; }
    StringAction& getNameAction() { return _nameAction; }
    PositionAction& getPositionAction() { return _positionAction; }
    DecimalAction& getScaleAction() { return _scaleAction; }
    ToggleAction& getFlipHorizontalAction() { return _flipHorizontalAction; }
    ToggleAction& getFlipVerticalAction() { return _flipVerticalAction; }

protected:
    Layer*          _layer;                 /** Pointer to owning layer */
    ToggleAction    _visibleAction;         /** Visible action */
    StringAction    _datasetNameAction;     /** Dataset name action */
    ColorAction     _colorAction;           /** Color action */
    StringAction    _nameAction;            /** Name action */
    PositionAction  _positionAction;        /** Position action */
    DecimalAction   _scaleAction;           /** Scale action */
    ToggleAction    _flipHorizontalAction; /** Flip horizontal action */
    ToggleAction    _flipVerticalAction;   /** Flip vertical action */
};

Q_DECLARE_METATYPE(GeneralAction)

inline const auto generalActionMetaTypeId = qRegisterMetaType<GeneralAction*>("GeneralAction");