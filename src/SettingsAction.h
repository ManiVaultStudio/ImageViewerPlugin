#pragma once

#include "LayersAction.h"
#include "SelectionAction.h"

class ImageViewerPlugin;

using namespace hdps::gui;

/**
 * Settings action class
 *
 * Action class for image viewer plugin settings (panel on the right of the view)
 *
 * @author Thomas Kroes
 */
class SettingsAction : public WidgetAction
{
public:

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param settingsAction Pointer to settings action
         */
        Widget(QWidget* parent, SettingsAction* settingsAction);

        /** Return preferred size */
        QSize sizeHint() const override {
            return QSize(350, 100);
        }

    protected:
        friend class SettingsAction;
    };

protected:

    /**
     * Get widget representation of the settings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    SettingsAction(ImageViewerPlugin& imageViewerPlugin);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /** Get reference to the image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin() { return _imageViewerPlugin; };

public: // Action getters

    LayersAction& getLayersAction() { return _layersAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;     /** Reference to image viewer plugin */
    LayersAction            _layersAction;          /** Layers action */
};
