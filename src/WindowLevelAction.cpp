#include "WindowLevelAction.h"
#include "ChannelAction.h"
#include "Application.h"

#include <QGridLayout>

using namespace hdps;

WindowLevelAction::WindowLevelAction(ChannelAction& channelAction) :
    WidgetAction(&channelAction),
    _channelAction(channelAction),
    _windowAction(this, "Window", 0.0f, 1.0f, 1.0f, 1.0f, 1),
    _levelAction(this, "Level", 0.0f, 1.0f, 0.5f, 0.5f, 1)
{
    setText("Window/level Settings");
    setIcon(Application::getIconFont("FontAwesome").getIcon("adjust"));

    _windowAction.setWidgetFlags(DecimalAction::All);
    _levelAction.setWidgetFlags(DecimalAction::All);

    _windowAction.setToolTip("Window");
    _levelAction.setToolTip("Level");
}

WindowLevelAction::Widget::Widget(QWidget* parent, WindowLevelAction* windowLevelAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, windowLevelAction, state)
{
    auto layout = new QGridLayout();

    layout->addWidget(windowLevelAction->getWindowAction().createLabelWidget(this), 0, 0);
    layout->addWidget(windowLevelAction->getWindowAction().createWidget(this), 0, 1);

    layout->addWidget(windowLevelAction->getLevelAction().createLabelWidget(this), 1, 0);
    layout->addWidget(windowLevelAction->getLevelAction().createWidget(this), 1, 1);

    setPopupLayout(layout);
}
