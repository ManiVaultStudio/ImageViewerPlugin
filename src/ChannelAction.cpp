#include "ChannelAction.h"

#include <QHBoxLayout>
#include <QCheckBox>

ChannelAction::ChannelAction(LayerImageAction& layerImageAction, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&layerImageAction)),
    _layerImageAction(layerImageAction),
    _enabledAction(this, ""),
    _dimensionAction(this, "Dimension"),
    _windowLevelAction(*this)
{
    setText(name);
}

ChannelAction::Widget::Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, channelAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    auto checkBox = new QCheckBox();

    if (channelAction->hasWidgetFlag(ChannelAction::CheckBox))
        layout->addWidget(channelAction->getEnabledAction().createWidget(this));

    if (channelAction->hasWidgetFlag(ChannelAction::ComboBox))
        layout->addWidget(channelAction->getDimensionAction().createWidget(this));

    if (channelAction->hasWidgetFlag(WidgetFlag::WindowLevelWidget))
        layout->addWidget(channelAction->getWindowLevelAction().createCollapsedWidget(this));

    if (channelAction->hasWidgetFlag(ChannelAction::ResetPushButton))
        layout->addWidget(channelAction->getDimensionAction().createResetButton(this));

    setLayout(layout);
}
