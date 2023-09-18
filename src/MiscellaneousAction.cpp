#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title, Layer& layer) :
    GroupAction(parent, title),
    _layer(layer),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI"),
    _roiDetailAction(this, "Detail ROI")
{
    setText("Miscellaneous");

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiDetailAction.setToolTip("ROI from another Image viewer");

    _roiLayerAction.setAxisNames("X1: ", "X2: ", "Y1: ", "Y2: ");
    _roiViewAction.setAxisNames("X1: ", "X2: ", "Y1: ", "Y2: ");
    _roiDetailAction.setAxisNames("X1: ", "X2: ", "Y1: ", "Y2: ");

    addAction(&_roiLayerAction);
    addAction(&_roiViewAction);
    addAction(&_roiDetailAction);

    _roiLayerAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
    _roiViewAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
    _roiDetailAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
}

// ######################
//      Helper Actions
// ######################


IntegralRectangleAction::IntegralRectangleAction(QObject* parent, const QString& title, const QRect& rectangle /*= QRect()*/) :
    RectangleAction<QRect, IntegralRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };

    connect(this, &IntegralRectangleAction::rectangleChanged, [this](const QRect& rectangle) {
        getRangeAction(Axis::X).getRangeMinAction().setMinimum(rectangle.left());
        getRangeAction(Axis::X).getRangeMinAction().setMaximum(rectangle.left());
        getRangeAction(Axis::X).getRangeMinAction().setValue(rectangle.left());

        getRangeAction(Axis::X).getRangeMaxAction().setMinimum(rectangle.right());
        getRangeAction(Axis::X).getRangeMaxAction().setMaximum(rectangle.right());
        getRangeAction(Axis::X).getRangeMaxAction().setValue(rectangle.right());

        getRangeAction(Axis::Y).getRangeMinAction().setMinimum(rectangle.bottom());
        getRangeAction(Axis::Y).getRangeMinAction().setMaximum(rectangle.bottom());
        getRangeAction(Axis::Y).getRangeMinAction().setValue(rectangle.bottom());

        getRangeAction(Axis::Y).getRangeMaxAction().setMinimum(rectangle.top());
        getRangeAction(Axis::Y).getRangeMaxAction().setMaximum(rectangle.top());
        getRangeAction(Axis::Y).getRangeMaxAction().setValue(rectangle.top());

        });
}

void IntegralRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicIntegralRectangleAction = dynamic_cast<IntegralRectangleAction*>(publicAction);

    Q_ASSERT(publicIntegralRectangleAction != nullptr);

    if (publicIntegralRectangleAction == nullptr)
        return;

    if (recursive) {
        hdps::actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::X), &publicIntegralRectangleAction->getRangeAction(Axis::X), recursive);
        hdps::actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::Y), &publicIntegralRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRect, IntegralRangeAction>::connectToPublicAction(publicAction, recursive);
}

void IntegralRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        hdps::actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::X), recursive);
        hdps::actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRect, IntegralRangeAction>::disconnectFromPublicAction(recursive);
}

void IntegralRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    RectangleAction<QRect, IntegralRangeAction>::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap IntegralRectangleAction::toVariantMap() const
{
    auto variantMap = RectangleAction<QRect, IntegralRangeAction>::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}

DecimalRectangleAction::DecimalRectangleAction(QObject* parent, const QString& title, const QRectF& rectangle /*= QRectF()*/) :
    RectangleAction<QRectF, DecimalRangeAction>(parent, title, rectangle)
{
    _rectangleChanged = [this]() -> void { emit rectangleChanged(getRectangle()); };

    connect(this, &DecimalRectangleAction::rectangleChanged, [this](const QRectF& rectangle) {
        getRangeAction(Axis::X).getRangeMinAction().setMinimum(rectangle.left());
        getRangeAction(Axis::X).getRangeMinAction().setMaximum(rectangle.left());
        getRangeAction(Axis::X).getRangeMinAction().setValue(rectangle.left());

        getRangeAction(Axis::X).getRangeMaxAction().setMinimum(rectangle.right());
        getRangeAction(Axis::X).getRangeMaxAction().setMaximum(rectangle.right());
        getRangeAction(Axis::X).getRangeMaxAction().setValue(rectangle.right());

        getRangeAction(Axis::Y).getRangeMinAction().setMinimum(rectangle.bottom());
        getRangeAction(Axis::Y).getRangeMinAction().setMaximum(rectangle.bottom());
        getRangeAction(Axis::Y).getRangeMinAction().setValue(rectangle.bottom());

        getRangeAction(Axis::Y).getRangeMaxAction().setMinimum(rectangle.top());
        getRangeAction(Axis::Y).getRangeMaxAction().setMaximum(rectangle.top());
        getRangeAction(Axis::Y).getRangeMaxAction().setValue(rectangle.top());

        });

}

void DecimalRectangleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDecimalRectangleAction = dynamic_cast<DecimalRectangleAction*>(publicAction);

    Q_ASSERT(publicDecimalRectangleAction != nullptr);

    if (publicDecimalRectangleAction == nullptr)
        return;

    if (recursive) {
        hdps::actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::X), &publicDecimalRectangleAction->getRangeAction(Axis::X), recursive);
        hdps::actions().connectPrivateActionToPublicAction(&getRangeAction(Axis::Y), &publicDecimalRectangleAction->getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRectF, DecimalRangeAction>::connectToPublicAction(publicAction, recursive);
}

void DecimalRectangleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        hdps::actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::X), recursive);
        hdps::actions().disconnectPrivateActionFromPublicAction(&getRangeAction(Axis::Y), recursive);
    }

    RectangleAction<QRectF, DecimalRangeAction>::disconnectFromPublicAction(recursive);
}

void DecimalRectangleAction::fromVariantMap(const QVariantMap& variantMap)
{
    RectangleAction<QRectF, DecimalRangeAction>::fromVariantMap(variantMap);

    getRangeAction(Axis::X).fromParentVariantMap(variantMap);
    getRangeAction(Axis::Y).fromParentVariantMap(variantMap);
}

QVariantMap DecimalRectangleAction::toVariantMap() const
{
    auto variantMap = RectangleAction<QRectF, DecimalRangeAction>::toVariantMap();

    getRangeAction(Axis::X).insertIntoVariantMap(variantMap);
    getRangeAction(Axis::Y).insertIntoVariantMap(variantMap);

    return variantMap;
}
