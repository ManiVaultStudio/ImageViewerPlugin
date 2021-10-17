#pragma once

#include "actions/WidgetAction.h"
#include "actions/GroupAction.h"
#include "actions/DecimalAction.h"
#include "actions/ColorAction.h"
#include "actions/ToggleAction.h"

using namespace hdps::gui;

class ImageViewerPlugin;

/**
 * Global view settings action class
 *
 * Settings action class for global view settings
 *
 * @author Thomas Kroes
 */
class GlobalViewSettingsAction : public WidgetAction
{
    Q_OBJECT

protected: // Widget

/** Widget class for subset action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param globalViewSettingsAction Pointer to global view settings action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, GlobalViewSettingsAction* globalViewSettingsAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the global view settings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    GlobalViewSettingsAction(ImageViewerPlugin& imageViewerPlugin);

public: /** Action getters */

    GroupAction& getGroupAction() { return _groupAction; }
    DecimalAction& getZoomMarginAction() { return _zoomMarginAction; }
    ColorAction& getBackgroundColorAction() { return _backgroundColorAction; }

protected:
    ImageViewerPlugin&  _imageViewerPlugin;         /** Reference to image viewer plugin */
    GroupAction         _groupAction;               /** Group action */
    DecimalAction       _zoomMarginAction;          /** Margin around layers extents action */
    ColorAction         _backgroundColorAction;     /** Background color action action */
    ToggleAction        _animationEnabledAction;    /** Animation on/off action */
};
