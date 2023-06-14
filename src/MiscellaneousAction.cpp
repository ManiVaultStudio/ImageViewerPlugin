#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI")
{
    addAction(&_roiLayerAction);
    addAction(&_roiViewAction);

    _roiLayerAction.setDefaultWidgetFlags(GroupAction::Vertical);
    _roiViewAction.setDefaultWidgetFlags(GroupAction::Vertical);

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
}
