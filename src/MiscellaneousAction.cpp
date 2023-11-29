#include "MiscellaneousAction.h"
#include "Layer.h"

#include <PointData/PointData.h>

#include <QRect>

#include <utility>

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title, Layer* layer) :
    GroupAction(parent, title),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI"),
    _timer(),
    _viewROI(),
    _layer(layer)
{
    setText("Miscellaneous");

    _roiLayerAction.setToolTip("Layer region of interest discrete image coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");
    _roiViewAction.setToolTip("View region of interest in fractional world coordinates (bottom-left:x, bottom-left:y, top-right:x, top-right:y)");

    addAction(&_roiLayerAction);
    addAction(&_roiViewAction);

    _roiLayerAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, true);
    _roiViewAction.setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, true);

    connect(&_timer, &QTimer::timeout, this, [this]() {
        if (!_roiViewAction.isConnected())
            return;

        QRectF viewROI;

        auto left = _roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMinAction().getValue();
        auto top = _roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMaxAction().getValue();

        // Hack: Range actions do not allow to set the min value larger than the max value
        if (_layer->_sourceDataset.get<Points>()->getProperty("_viewRoi_FLIP").toBool())
            std::swap(left, top);

        viewROI.setLeft(left);
        viewROI.setTop(top);

        assert(_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMinAction().getValue() == _roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMaxAction().getValue());

        const float w = _roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMinAction().getValue();
        viewROI.setWidth(w);
        viewROI.setHeight(w);

        if (viewROI == _viewROI)
            return;

        _viewROI = viewROI;
        emit viewROIChanged(_viewROI);

        });

    _timer.start(30);
}
