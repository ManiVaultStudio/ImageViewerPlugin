#include "MiscellaneousAction.h"
#include "Layer.h"
#include "LayersAction.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(Layer& layer) :
    GroupAction(&layer, true),
    _layer(layer),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI")
{
    setText("Miscellaneous");

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
}
