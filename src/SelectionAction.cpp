#include "SelectionAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"

#include <CoreInterface.h>

#include <util/PixelSelectionTool.h>

#include <Application.h>

using namespace mv;
using namespace mv::util;

const auto allowedPixelSelectionTypes = PixelSelectionTypes({
    PixelSelectionType::Rectangle,
    PixelSelectionType::Brush,
    PixelSelectionType::Lasso,
    PixelSelectionType::Polygon,
    PixelSelectionType::Sample,
    PixelSelectionType::ROI
});

SelectionAction::SelectionAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _layer(nullptr),
    _targetWidget(nullptr),
    _pixelSelectionAction(this, "Pixel Selection"),
    _pixelSelectionTool(nullptr),
    _showRegionAction(this, "Show selected region", false)
{
    setIcon(mv::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _showRegionAction.setVisible(false);

    addAction(&_pixelSelectionAction.getTypeAction());
    addAction(&_pixelSelectionAction.getBrushRadiusAction());
    addAction(&_pixelSelectionAction.getModifierAction());
    addAction(&_pixelSelectionAction.getOverlayColorAction());
    addAction(&_pixelSelectionAction.getOverlayOpacityAction());
    addAction(&_pixelSelectionAction.getNotifyDuringSelectionAction());
}

void SelectionAction::initialize(Layer* layer, QWidget* targetWidget, PixelSelectionTool* pixelSelectionTool)
{
    Q_ASSERT(layer != nullptr);
    Q_ASSERT(pixelSelectionTool != nullptr);

    if (layer == nullptr || pixelSelectionTool == nullptr)
        return;

    _layer              = layer;
    _targetWidget       = targetWidget;
    _pixelSelectionTool = pixelSelectionTool;

    _pixelSelectionAction.initialize(targetWidget, _pixelSelectionTool, allowedPixelSelectionTypes);

    connect(&_pixelSelectionAction.getOverlayColorAction(), &ColorAction::colorChanged, _layer, &Layer::invalidate);
    connect(&_pixelSelectionAction.getOverlayOpacityAction(), &DecimalAction::valueChanged, _layer, &Layer::invalidate);
    connect(&_showRegionAction, &ToggleAction::toggled, _layer, &Layer::invalidate);

    const auto updateInteractionActions = [this]() -> void {
        const auto inSelectionMode  = _layer->getImageViewerPlugin().getImageViewerWidget().getInteractionMode() == ImageViewerWidget::InteractionMode::Selection;
        const auto enable           = inSelectionMode && _layer->isActive();
        
        _pixelSelectionAction.getRectangleAction().setEnabled(enable);
        _pixelSelectionAction.getBrushAction().setEnabled(enable);
        _pixelSelectionAction.getLassoAction().setEnabled(enable);
        _pixelSelectionAction.getPolygonAction().setEnabled(enable);
        _pixelSelectionAction.getSampleAction().setEnabled(enable);
        _pixelSelectionAction.getModifierAction().setEnabled(enable);
    };

    updateInteractionActions();

    connect(&_layer->getImageViewerPlugin().getImageViewerWidget(), &ImageViewerWidget::interactionModeChanged, this, updateInteractionActions);
}

QRect SelectionAction::getImageSelectionRectangle() const
{
    if (_layer == nullptr)
        return {};

    return _layer->getImageSelectionRectangle();
}

void SelectionAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicSelectionAction = dynamic_cast<SelectionAction*>(publicAction);

    Q_ASSERT(publicSelectionAction != nullptr);

    if (publicSelectionAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_pixelSelectionAction, &publicSelectionAction->getPixelSelectionAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_showRegionAction, &publicSelectionAction->getShowRegionAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void SelectionAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_pixelSelectionAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_showRegionAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void SelectionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _pixelSelectionAction.fromParentVariantMap(variantMap);
    _showRegionAction.fromParentVariantMap(variantMap);
}

QVariantMap SelectionAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    _pixelSelectionAction.insertIntoVariantMap(variantMap);
    _showRegionAction.insertIntoVariantMap(variantMap);

    return variantMap;
}