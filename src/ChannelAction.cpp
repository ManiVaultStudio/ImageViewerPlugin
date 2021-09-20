#include "ChannelAction.h"

#include <QHBoxLayout>

ChannelAction::ChannelAction(LayerImageAction& layerImageAction, const QString& name) :
    WidgetAction(reinterpret_cast<QObject*>(&layerImageAction)),
    _layerImageAction(layerImageAction),
    _dimensionAction(this, "Dimension")
{
    setText(name);
}


ChannelAction::Widget::Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, channelAction, state)
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(channelAction->getDimensionAction().createWidget(this));

    setLayout(layout);
}
