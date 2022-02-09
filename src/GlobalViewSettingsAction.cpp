#include "GlobalViewSettingsAction.h"
#include "ImageViewerPlugin.h"

#include <Application.h> 

#include <QHBoxLayout>

using namespace hdps;

GlobalViewSettingsAction::GlobalViewSettingsAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(&imageViewerPlugin)),
    _imageViewerPlugin(imageViewerPlugin),
    _groupAction(this),
    _zoomMarginAction(this, "Zoom margin", 1.0f, 1000.0f, 100.0f, 100.0f),
    _backgroundColorAction(this, "Background color", QColor(50, 50, 50), QColor(50, 50, 50)),
    _animationEnabledAction(this, "Animation", true, true),
    _smartZoomAction(this, "Smart zoom", true, true)
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setText("Global view settings");

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

    // Update renderer zoom margin
    const auto updateZoomMargin = [this, &imageViewerWidget]() {
        imageViewerWidget.getRenderer().setZoomMargin(_zoomMarginAction.getValue());
        imageViewerWidget.getRenderer().setZoomRectangle(imageViewerWidget.getWorldBoundingRectangle());
    };

    // Update the viewer background color
    const auto updateBackgroundColor = [this, &imageViewerWidget]() {
        imageViewerWidget.setBackgroundColor(_backgroundColorAction.getColor());
    };

    // Update animation enabled
    const auto updateAnimation = [this, &imageViewerWidget]() {
        imageViewerWidget.getRenderer().setAnimationEnabled(_animationEnabledAction.isChecked());
    };

    connect(&_zoomMarginAction, &DecimalAction::valueChanged, this, updateZoomMargin);
    connect(&_backgroundColorAction, &ColorAction::colorChanged, this, updateBackgroundColor);
    connect(&_animationEnabledAction, &ToggleAction::toggled, this, updateAnimation);

    // Do initial updates
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
