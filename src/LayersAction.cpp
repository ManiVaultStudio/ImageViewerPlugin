#include "LayersAction.h"
#include "Application.h"

using namespace hdps::gui;

LayersAction::LayersAction(SettingsAction& settingsAction) :
    WidgetAction(reinterpret_cast<QObject*>(&settingsAction)),
    _settingsAction(settingsAction)
{
}
