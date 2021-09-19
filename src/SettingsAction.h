#pragma once

#include "LayersAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Settings action class
 *
 * Action class for image viewer plugin settings (panel on the right of the view)
 *
 * @author Thomas Kroes
 */
class SettingsAction : public WidgetAction
{
public:

    /** 
     * Constructor
     * @param imageViewerPlugin Pointer to image viewer plugin
     */
    SettingsAction(ImageViewerPlugin* imageViewerPlugin);

public: /** Action getters */

    LayersAction& getLayersAction() { return _layersAction; }
    //LayerAction& getLayerAction() { return _layerAction; }

protected:
    ImageViewerPlugin*  _imageViewerPlugin;     /** Pointer to image viewer plugin */
    LayersAction        _layersAction;          /** Layers action */
};
