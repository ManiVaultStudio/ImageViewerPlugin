#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _roiSelectionAction(this, "Selection"),
    _roiLayerAction(this, "Layer ROI"),
    //_roiLayerGroupAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI")
    //_roiViewGroupAction(this, "View ROI")
{

    _roiLayerAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, true);
    _roiViewAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, true);
    _roiSelectionAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, true);

    _roiSelectionAction.setRectangle({0, 0, 0, 0});

    addAction(&_roiLayerAction);
    addAction(&_roiViewAction);
    addAction(&_roiSelectionAction);

    //_roiLayerGroupAction.setDefaultWidgetFlags(GroupAction::Vertical | GroupAction::NoMargins);
    //_roiLayerGroupAction.setEnabled(true);
    //_roiLayerGroupAction.setShowLabels(false);
    //_roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::X).getRangeMinAction(), IntegralAction::LineEdit);
    //_roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::X).getRangeMaxAction(), IntegralAction::LineEdit);
    //_roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::Y).getRangeMinAction(), IntegralAction::LineEdit);
    //_roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::Y).getRangeMaxAction(), IntegralAction::LineEdit);

    //_roiViewGroupAction.setDefaultWidgetFlags(GroupAction::Vertical | GroupAction::NoMargins);
    //_roiViewGroupAction.setEnabled(false);
    //_roiViewGroupAction.setShowLabels(false);
    //_roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMinAction(), IntegralAction::LineEdit);
    //_roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMaxAction(), IntegralAction::LineEdit);
    //_roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMinAction(), IntegralAction::LineEdit);
    //_roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMaxAction(), IntegralAction::LineEdit);

    //addAction(&_roiLayerGroupAction);
    //addAction(&_roiViewGroupAction);

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiSelectionAction.setToolTip("Selection ROI");
}
