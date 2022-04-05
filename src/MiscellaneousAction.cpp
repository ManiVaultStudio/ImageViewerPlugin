#include "MiscellaneousAction.h"
#include "Layer.h"
#include "LayersAction.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(Layer& layer) :
    GroupAction(&layer, true),
    _layer(layer),
    _roiAction(this, "Image ROI")
{
    setText("Miscellaneous");

    _roiAction.setToolTip("Visible rectangle in image coordinates");
}
