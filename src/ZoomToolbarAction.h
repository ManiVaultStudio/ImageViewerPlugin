#pragma once

#include <actions/WidgetAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>

#include "SubsetAction.h"

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
class ZoomToolbarAction : public WidgetAction
{
public:

    /** Widget class for zoom toolbar action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param zoomToolbarAction Pointer to zoom toolbar action
         */
        Widget(QWidget* parent, ZoomToolbarAction* zoomToolbarAction);

    protected:
        friend class ZoomToolbarAction;
    };

protected:

    /**
     * Get widget representation of the zoom toolbar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param imageViewerWidget Reference to image viewer plugin
     */
    ZoomToolbarAction(ImageViewerPlugin& imageViewerPlugin);

    /** Get reference to image viewer widget */
    ImageViewerWidget& getImageViewerWidget();

public: // Action getters

    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    TriggerAction& getExportToImageAction() { return _exportToImageAction; }

protected:
    ImageViewerPlugin&  _imageViewerPlugin;         /** Reference to image viewer plugin */
    TriggerAction       _zoomOutAction;             /** Zoom out action */
    DecimalAction       _zoomPercentageAction;      /** Zoom action */
    TriggerAction       _zoomInAction;              /** Zoom in action */
    TriggerAction       _zoomExtentsAction;         /** Zoom extents action */
    TriggerAction       _exportToImageAction;       /** Export to image action */

    static const float zoomDeltaPercentage;
};
