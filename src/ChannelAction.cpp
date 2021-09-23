#include "ChannelAction.h"

#include <QHBoxLayout>
#include <QCheckBox>

ChannelAction::ChannelAction(LayerImageAction& layerImageAction, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&layerImageAction)),
    _layerImageAction(layerImageAction),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(*this)
{
    setText(name);

    connect(&_dimensionAction, &OptionAction::resettableChanged, this, [this]() {
        setResettable(isResettable());
    });
}

bool ChannelAction::isResettable() const
{
    return _dimensionAction.isResettable();
}

void ChannelAction::reset()
{
    _dimensionAction.reset();
}

ChannelAction::Widget::Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, channelAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(0);

    auto checkBox = new QCheckBox();

    if (channelAction->hasWidgetFlag(ChannelAction::ComboBox))
        layout->addWidget(channelAction->getDimensionAction().createWidget(this));

    if (channelAction->hasWidgetFlag(WidgetFlag::WindowLevelWidget))
        layout->addWidget(channelAction->getWindowLevelAction().createCollapsedWidget(this));

    if (channelAction->hasWidgetFlag(ChannelAction::ResetPushButton))
        layout->addWidget(channelAction->createResetButton(this));

    setLayout(layout);
}
