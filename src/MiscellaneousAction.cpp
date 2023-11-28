#include "MiscellaneousAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"

#include <QRect>

MiscellaneousAction::MiscellaneousAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _roiLayerAction(this, "Layer ROI"),
    _roiViewAction(this, "View ROI"),
    _timer(),
    _viewROI()
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
        viewROI.setX(_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMinAction().getValue());
        viewROI.setY(_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::Y).getRangeMinAction().getValue());
        const float w = std::abs(_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMinAction().getValue()) + std::abs(_roiViewAction.getRangeAction(DecimalRectangleAction::Axis::X).getRangeMaxAction().getValue());
        viewROI.setWidth(w);
        viewROI.setHeight(w);

        if (viewROI == _viewROI)
            return;

        _viewROI = viewROI;

        qDebug() << " -- MiscellaneousAction -- ";
        qDebug() << "viewROI: " << viewROI;

        //emit viewROIChanged(_viewROI);

        });

    _timer.start(30);
}
