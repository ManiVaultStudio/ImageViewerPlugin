#pragma once

#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"
#include "actions/StringAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Subset action class
 *
 * Action class for creating images/points subsets
 *
 * @author Thomas Kroes
 */
class SubsetAction : public TriggerAction
{
public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    SubsetAction(ImageViewerPlugin& imageViewerPlugin);

    /** Get reference to image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin() { return _imageViewerPlugin; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;     /** Reference to image viewer plugin */
};
