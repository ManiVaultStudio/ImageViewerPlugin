#pragma once

#include "actions/WidgetAction.h"
#include "actions/TriggerAction.h"

#include "ImageData/Images.h"

using namespace hdps::gui;
using namespace hdps::util;

class GeneralAction;

/**
 * Zoom action class
 *
 * Zoom to extents/selection action class
 *
 * @author Thomas Kroes
 */
class ZoomAction : public WidgetAction
{
    Q_OBJECT

protected:

    /**
     * Get widget representation of the zoom action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /** 
     * Constructor
     * @param generalAction Reference to general action
     */
    ZoomAction(GeneralAction& generalAction);

public: /** Action getters */

    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    TriggerAction& getZoomSelectionAction() { return _zoomSelectionAction; }

protected:
    GeneralAction&      _generalAction;         /** Reference to general action */
    TriggerAction       _zoomExtentsAction;     /** Zoom extents action */
    TriggerAction       _zoomSelectionAction;   /** Zoom to selection action */
};
