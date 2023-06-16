#pragma once

#include <actions/GroupAction.h>
#include <actions/IntegralRectangleAction.h>
#include <actions/DecimalRectangleAction.h>
#include <actions/VerticalGroupAction.h>

class Layer;

using namespace hdps::gui;

/**
 * Miscellaneous action class
 *
 * Group action class for various actions
 *
 * @author Thomas Kroes
 */
class MiscellaneousAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE MiscellaneousAction(QObject* parent, const QString& title);

public: // Action getters

    IntegralRectangleAction& getRoiLayerAction() { return _roiLayerAction; }
    DecimalRectangleAction& getRoiViewAction() { return _roiViewAction; }

protected:
    IntegralRectangleAction _roiLayerAction;        /** Layer region of interest action */
    VerticalGroupAction     _roiLayerGroupAction;   /** Layer region of interest group action */
    DecimalRectangleAction  _roiViewAction;         /** View region of interest action */
    VerticalGroupAction     _roiViewGroupAction;    /** View region of interest group action */

};

Q_DECLARE_METATYPE(MiscellaneousAction)

inline const auto miscellaneousActionMetaTypeId = qRegisterMetaType<MiscellaneousAction*>("MiscellaneousAction");