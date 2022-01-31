#include "PositionAction.h"
#include "GeneralAction.h"
#include "Layer.h"

#include <QHBoxLayout>

using namespace hdps;

PositionAction::PositionAction(GeneralAction& generalAction) :
    WidgetAction(reinterpret_cast<QObject*>(&generalAction)),
    _generalAction(generalAction),
    _xAction(this, "X position", -100000.0f, 100000.0f, 0.0f, 0.0f),
    _yAction(this, "Y position", -100000.0f, 100000.0f, 0.0f, 0.0f)
{
    setText("Position");
    setToolTip("Layer position");
    setMayReset(true);

    // Set tooltips
    _xAction.setToolTip("Layer x-position");
    _yAction.setToolTip("Layer y-position");

    // Configure position widgets
    _xAction.setDefaultWidgetFlags(DecimalAction::SpinBox);
    _yAction.setDefaultWidgetFlags(DecimalAction::SpinBox);

    const auto notifyChanged = [this]() -> void {
        emit resettableChanged(isResettable());
        emit changed();
    };

    connect(&_xAction, &DecimalAction::valueChanged, this, notifyChanged);
    connect(&_yAction, &DecimalAction::valueChanged, this, notifyChanged);
}

bool PositionAction::isResettable() const
{
    return _xAction.isResettable() || _yAction.isResettable();
}

void PositionAction::reset()
{
    _xAction.reset();
    _yAction.reset();
}

QWidget* PositionAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    layout->addWidget(_xAction.createWidget(widget));
    layout->addWidget(_yAction.createWidget(widget));

    widget->setLayout(layout);

    return widget;
}