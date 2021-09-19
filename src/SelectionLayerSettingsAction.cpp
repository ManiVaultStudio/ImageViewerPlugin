#include "SelectionLayerSettingsAction.h"

SelectionLayerSettingsAction::SelectionLayerSettingsAction(LayerSelectionAction& selectionLayerAction) :
    GroupAction(reinterpret_cast<QObject*>(&selectionLayerAction)),
    _selectionLayerAction(selectionLayerAction),
    _images(),
    _selectionTypeAction(this, "Selection type"),
    _selectionModifierAction(this, "Selection modifier"),
    _brushRadiusAction(this, "Brush radius"),
    _overlayColorAction(this, "Selection color")
{
}
