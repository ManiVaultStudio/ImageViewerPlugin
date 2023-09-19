#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title, Layer& layer) :
    GroupAction(parent, title),
    _layer(layer),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI"),
    _roiDetailAction(this, "Detail ROI")
{
    setText("Miscellaneous");

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiDetailAction.setToolTip("ROI from another Image viewer");

    addAction(&_roiLayerAction);
    addAction(&_roiViewAction);
    addAction(&_roiDetailAction);

    _roiLayerAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
    _roiViewAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
    _roiDetailAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All);
}
