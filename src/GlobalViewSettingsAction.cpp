#include "GlobalViewSettingsAction.h"
#include "ImageViewerPlugin.h"

#include <Application.h> 

#include <QHBoxLayout>

using namespace hdps;

ViewSettingsAction::ViewSettingsAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _imageViewerPlugin(nullptr),
    _zoomMarginAction(this, "Zoom margin", 1.0f, 1000.0f, 100.0f, 100.0f),
    _backgroundColorAction(this, "Background color", QColor(50, 50, 50), QColor(50, 50, 50)),
    _animationEnabledAction(this, "Animation", true, true),
    _smartZoomAction(this, "Smart zoom", true, true)
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    _zoomMarginAction.setToolTip("Zoom margin around the layers extents");
    _backgroundColorAction.setToolTip("Background color of the viewer");
    _animationEnabledAction.setToolTip("Enable animations");
    _smartZoomAction.setToolTip("Automatically zoom when selecting layers");
    
    addAction(&_zoomMarginAction);
    addAction(&_backgroundColorAction);
    addAction(&_animationEnabledAction);
    addAction(&_smartZoomAction);

    _zoomMarginAction.setSuffix("px");
    _zoomMarginAction.setUpdateDuringDrag(false);
}

void ViewSettingsAction::initialize(ImageViewerPlugin* imageViewerPlugin)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;

    auto& imageViewerWidget = _imageViewerPlugin->getImageViewerWidget();

    const auto updateZoomMargin = [this, &imageViewerWidget]() {
        imageViewerWidget.getRenderer().setZoomMargin(_zoomMarginAction.getValue());
        imageViewerWidget.getRenderer().setZoomRectangle(imageViewerWidget.getWorldBoundingRectangle());
    };

    updateZoomMargin();

    const auto updateBackgroundColor = [this, &imageViewerWidget]() {
        imageViewerWidget.setBackgroundColor(_backgroundColorAction.getColor());
    };

    updateBackgroundColor();

    const auto updateAnimation = [this, &imageViewerWidget]() {
        imageViewerWidget.getRenderer().setAnimationEnabled(_animationEnabledAction.isChecked());
    };
    
    updateAnimation();

    connect(&_zoomMarginAction, &DecimalAction::valueChanged, this, updateZoomMargin);
    connect(&_backgroundColorAction, &ColorAction::colorChanged, this, updateBackgroundColor);
    connect(&_animationEnabledAction, &ToggleAction::toggled, this, updateAnimation);
}

void ViewSettingsAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicViewSettingsAction = dynamic_cast<ViewSettingsAction*>(publicAction);

    Q_ASSERT(publicViewSettingsAction != nullptr);

    if (publicViewSettingsAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_zoomMarginAction, &publicViewSettingsAction->getZoomMarginAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_backgroundColorAction, &publicViewSettingsAction->getBackgroundColorAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_animationEnabledAction, &publicViewSettingsAction->getAnimationEnabledAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_smartZoomAction, &publicViewSettingsAction->getSmartZoomAction(), recursive);
    }
}

void ViewSettingsAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_zoomMarginAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_backgroundColorAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_animationEnabledAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_smartZoomAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void ViewSettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _zoomMarginAction.fromParentVariantMap(variantMap);
    _backgroundColorAction.fromParentVariantMap(variantMap);
    _animationEnabledAction.fromParentVariantMap(variantMap);
    _smartZoomAction.fromParentVariantMap(variantMap);
}

QVariantMap ViewSettingsAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _zoomMarginAction.insertIntoVariantMap(variantMap);
    _backgroundColorAction.insertIntoVariantMap(variantMap);
    _animationEnabledAction.insertIntoVariantMap(variantMap);
    _smartZoomAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
