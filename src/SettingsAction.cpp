#include "SettingsAction.h"
#include "ImageViewerPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ImageViewerPlugin* imageViewerPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(imageViewerPlugin)),
    _imageViewerPlugin(imageViewerPlugin),
    _layersAction(*this),
    _selectionAction(imageViewerPlugin, imageViewerPlugin->getImageViewerWidget()->getPixelSelectionTool())
{
}

SettingsAction::Widget::Widget(QWidget* parent, SettingsAction* settingsAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, settingsAction, state)
{
    setAutoFillBackground(true);
    setMinimumWidth(300);
    
    auto layout = new QVBoxLayout();

    layout->setMargin(4);
    layout->addWidget(settingsAction->getLayersAction().createWidget(this));

    setLayout(layout);
}
