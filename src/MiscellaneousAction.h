#pragma once

#include <actions/GroupAction.h>
#include <actions/IntegralRectangleAction.h>
#include <actions/DecimalRectangleAction.h>
#include <actions/VerticalGroupAction.h>

#include <QTimer>
#include <QRectF>

using namespace mv::gui;

class Layer;

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
    Q_INVOKABLE MiscellaneousAction(QObject* parent, const QString& title, Layer* layer);

public: // Setter
    void setViewROI(const QRectF& r) { _viewROI = r; }

public: // Action getters

    IntegralRectangleAction& getRoiLayerAction() { return _roiLayerAction; }
    DecimalRectangleAction& getRoiViewAction() { return _roiViewAction; }

signals:
    void viewROIChanged(const QRectF& rectangle);

protected:
    IntegralRectangleAction     _roiLayerAction;    /** Layer region of interest action */
    DecimalRectangleAction      _roiViewAction;     /** View region of interest action */

private:
    QTimer                      _timer;
    QRectF                      _viewROI;
    Layer*                      _layer;
};

Q_DECLARE_METATYPE(MiscellaneousAction)

inline const auto miscellaneousActionMetaTypeId = qRegisterMetaType<MiscellaneousAction*>("MiscellaneousAction");

