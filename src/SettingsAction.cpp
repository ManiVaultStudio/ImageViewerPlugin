#include "SettingsAction.h"
#include "ImageViewerPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _layersAction(*this)
{
}

SettingsAction::Widget::Widget(QWidget* parent, SettingsAction* settingsAction) :
    WidgetActionWidget(parent, settingsAction)
{
    setAutoFillBackground(true);
    setFixedWidth(350);
    
    auto layout = new QVBoxLayout();

    layout->setMargin(4);
    layout->addWidget(settingsAction->getLayersAction().createWidget(this));

    setLayout(layout);
}
