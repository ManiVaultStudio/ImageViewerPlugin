#include "PositionAction.h"
#include "GeneralAction.h"
#include "Layer.h"

using namespace mv;

PositionAction::PositionAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _xAction(this, "X", -100000.0f, 100000.0f, 0.0f, 0.0f),
    _yAction(this, "Y", -100000.0f, 100000.0f, 0.0f, 0.0f)
{
    setToolTip("Layer position");
    setDefaultWidgetFlags(GroupAction::Horizontal);
    setShowLabels(false);

    addAction(&_xAction);
    addAction(&_yAction);

    _xAction.setToolTip("Layer x-position");
    _yAction.setToolTip("Layer y-position");

    _xAction.setDefaultWidgetFlags(DecimalAction::SpinBox);
    _yAction.setDefaultWidgetFlags(DecimalAction::SpinBox);

    const auto notifyChanged = [this]() -> void {
        emit changed();
    };

    connect(&_xAction, &DecimalAction::valueChanged, this, notifyChanged);
    connect(&_yAction, &DecimalAction::valueChanged, this, notifyChanged);
}

void PositionAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicPositionAction = dynamic_cast<PositionAction*>(publicAction);

    Q_ASSERT(publicPositionAction != nullptr);

    if (publicPositionAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_xAction, &publicPositionAction->getXAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_yAction, &publicPositionAction->getYAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void PositionAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_xAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_yAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void PositionAction::fromVariantMap(const mv::VariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _xAction.fromParentVariantMap(variantMap);
    _yAction.fromParentVariantMap(variantMap);
}

mv::VariantMap PositionAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _xAction.insertIntoVariantMap(variantMap);
    _yAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
