#pragma once

#include "actions/Actions.h"

class QMenu;
class SettingsAction;

using namespace hdps::gui;

/**
 * Layers action class
 *
 * Action class for image layers display and interaction 
 *
 * @author Thomas Kroes
 */
class LayersAction : public WidgetAction
{
public:

    /** 
     * Constructor
     * @param settingsAction Reference to settings action
     */
    LayersAction(SettingsAction& settingsAction);

public: // Action getters

    //LayersAction& getLayersAction() { return _layersAction; }
    //LayerAction& getLayerAction() { return _layerAction; }

protected:
    SettingsAction&     _settingsAction;     /** Reference to settings action */
    //LayersAction        _layersAction;          /** Layers action */
    //LayerAction         _layerAction;           /** Layer action */
};
