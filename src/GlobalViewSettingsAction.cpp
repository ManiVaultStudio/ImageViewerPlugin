#include "GlobalViewSettingsAction.h"
#include "ImageViewerPlugin.h"
#include "Application.h"

#include <QHBoxLayout>

using namespace hdps;

GlobalViewSettingsAction::GlobalViewSettingsAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(&imageViewerPlugin)),
    _imageViewerPlugin(imageViewerPlugin),
    _groupAction(this),
    _zoomMarginAction(this, "Zoom margin"),
    _backgroundColorAction(this, "Background color"),
    _animationEnabledAction(this, "Animation"),
    _smartZoomAction(this, "Smart zoom")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setText("View settings");

    _zoomMarginAction.setToolTip("Zoom margin around the layers extents");
    _backgroundColorAction.setToolTip("Background color of the viewer");
    _animationEnabledAction.setToolTip("Enable animations");
    _smartZoomAction.setToolTip("Automatically zoom when selecting layers");
    
    _groupAction << _zoomMarginAction;
    _groupAction << _backgroundColorAction;
    _groupAction << _animationEnabledAction;
    _groupAction << _smartZoomAction;

    _zoomMarginAction.setSuffix("px");
    _zoomMarginAction.setUpdateDuringDrag(false);

    auto& imageViewerWidget = _imageViewerPlugin.getImageViewerWidget();

    // Settings prefix in the plugin
    const auto settingsPrefix = "ImageViewerPlugin/ViewSettings";

    // Default values
    const auto defaultZoomMargin        = 100.0f;
    const auto defaultBackgroundColor   = QColor(50, 50, 50);
    const auto defaultAnimationEnabled  = true;
    const auto defaultSmartZoom         = true;

    // Get action values from image viewer plugin settings
    const auto globalZoomMargin         = _imageViewerPlugin.getSetting(QString("%1/%2").arg(settingsPrefix, "/ZoomMargin"), defaultZoomMargin).toFloat();
    const auto globalBackgroundColor    = _imageViewerPlugin.getSetting(QString("%1/%2").arg(settingsPrefix, "/BackgroundColor"), defaultBackgroundColor).value<QColor>();
    const auto globalAnimationEnabled   = _imageViewerPlugin.getSetting(QString("%1/%2").arg(settingsPrefix, "/AnimationEnabled"), defaultAnimationEnabled).toBool();
    const auto globalSmartZoom          = _imageViewerPlugin.getSetting(QString("%1/%2").arg(settingsPrefix, "/SmartZoom"), defaultSmartZoom).toBool();

    // Initialize actions with default values
    _zoomMarginAction.initialize(0.0f, 250.0f, globalZoomMargin, defaultZoomMargin);
    _backgroundColorAction.initialize(globalBackgroundColor, defaultBackgroundColor);
    _animationEnabledAction.initialize(globalAnimationEnabled, defaultAnimationEnabled);
    _smartZoomAction.initialize(globalSmartZoom, defaultSmartZoom);

    // Update renderer zoom margin
    const auto updateZoomMargin = [this, &imageViewerWidget, settingsPrefix]() {

        // Update renderer zoom margin
        imageViewerWidget.getRenderer().setZoomMargin(_zoomMarginAction.getValue());
        imageViewerWidget.getRenderer().setZoomRectangle(imageViewerWidget.getWorldBoundingRectangle());

        // Save to plugin settings
        _imageViewerPlugin.setSetting("ImageViewerPlugin/ViewSettings/ZoomMargin", _zoomMarginAction.getValue());
    };

    // Update the viewer background color
    const auto updateBackgroundColor = [this, &imageViewerWidget, settingsPrefix]() {

        // Update viewer background color
        imageViewerWidget.setBackgroundColor(_backgroundColorAction.getColor());

        // Save to plugin settings
        _imageViewerPlugin.setSetting("ImageViewerPlugin/ViewSettings/BackgroundColor", _backgroundColorAction.getColor());
    };

    // Update animation enabled
    const auto updateAnimation = [this, &imageViewerWidget, settingsPrefix]() {

        // Update renderer animation enabled
        imageViewerWidget.getRenderer().setAnimationEnabled(_animationEnabledAction.isChecked());

        // Save to plugin settings
        _imageViewerPlugin.setSetting("ImageViewerPlugin/ViewSettings/AnimationEnabled", _animationEnabledAction.isChecked());
    };

    connect(&_zoomMarginAction, &DecimalAction::valueChanged, this, updateZoomMargin);
    connect(&_backgroundColorAction, &ColorAction::colorChanged, this, updateBackgroundColor);
    connect(&_animationEnabledAction, &ToggleAction::toggled, this, updateAnimation);

    // Do initial updates of the view
    updateZoomMargin();
    updateBackgroundColor();
    updateAnimation();
}

GlobalViewSettingsAction::Widget::Widget(QWidget* parent, GlobalViewSettingsAction* globalViewSettingsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, globalViewSettingsAction)
{
    auto layout = new QVBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    layout->addWidget(globalViewSettingsAction->getGroupAction().createWidget(this));

    setPopupLayout(layout);
}
