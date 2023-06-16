#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>
#include <actions/OptionAction.h>
#include <actions/HorizontalGroupAction.h>

class ImageViewerPlugin;
class ImageViewerWidget;

using namespace hdps::gui;

/**
 * Zoom toolbar action class
 *
 * Action class for zooming
 *
 * @author Thomas Kroes
 */
class ZoomToolbarAction : public HorizontalToolbarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ZoomToolbarAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p imageViewerPlugin
     * @param imageViewerPlugin Pointer to image viewer plugin
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin);

    /** Get reference to image viewer widget */
    ImageViewerWidget& getImageViewerWidget();

public: // Action getters

    ToggleAction& getPanAction() { return _navigationAction; }
    ToggleAction& getSelectAction() { return _selectAction; }
    OptionAction& getInteractionModeAction() { return _interactionModeAction; }
    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    TriggerAction& getZoomSelectionAction() { return _zoomSelectionAction; }
    TriggerAction& getExportToImageAction() { return _exportToImageAction; }

protected:
    ImageViewerPlugin*      _imageViewerPlugin;             /** Pointer to image viewer plugin */
    ToggleAction            _navigationAction;                     /** Pan interaction mode action */
    ToggleAction            _selectAction;                  /** Select interaction mode action */
    OptionAction            _interactionModeAction;         /** Interaction mode option action */
    HorizontalGroupAction   _interactionModeGroupAction;    /** Interaction mode option action */
    TriggerAction           _zoomOutAction;                 /** Zoom out action */
    DecimalAction           _zoomPercentageAction;          /** Zoom action */
    TriggerAction           _zoomInAction;                  /** Zoom in action */
    TriggerAction           _zoomExtentsAction;             /** Zoom extents action */
    TriggerAction           _zoomSelectionAction;           /** Zoom to selection extents action */
    TriggerAction           _exportToImageAction;           /** Export to image action */

    static const float zoomDeltaPercentage;
};

Q_DECLARE_METATYPE(ZoomToolbarAction)

inline const auto zoomToolbarActionMetaTypeId = qRegisterMetaType<ZoomToolbarAction*>("ZoomToolbarAction");