#include "SettingsAction.h"
#include "Application.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ImageViewerPlugin* imageViewerPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(imageViewerPlugin))
{
}
