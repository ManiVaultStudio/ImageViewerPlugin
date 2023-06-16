#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _roiLayerAction(this, "Layer ROI"),
    _roiLayerGroupAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI"),
    _roiViewGroupAction(this, "View ROI")
{
    _roiLayerGroupAction.setEnabled(false);
    _roiLayerGroupAction.setShowLabels(false);
    _roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::X).getRangeMinAction(), IntegralAction::LineEdit);
    _roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::X).getRangeMaxAction(), IntegralAction::LineEdit);
    _roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::Y).getRangeMinAction(), IntegralAction::LineEdit);
    _roiLayerGroupAction.addAction(&_roiLayerAction.getRangeAction(IntegralRectangleAction::Axis::Y).getRangeMaxAction(), IntegralAction::LineEdit);

    _roiViewGroupAction.setEnabled(false);
    _roiViewGroupAction.setShowLabels(false);
    _roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMinAction(), IntegralAction::LineEdit);
    _roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMaxAction(), IntegralAction::LineEdit);
    _roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMinAction(), IntegralAction::LineEdit);
    _roiViewGroupAction.addAction(&_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMaxAction(), IntegralAction::LineEdit);

    addAction(&_roiLayerGroupAction, GroupAction::Vertical | GroupAction::NoMargins);
    addAction(&_roiViewGroupAction, GroupAction::Vertical | GroupAction::NoMargins);

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
}
