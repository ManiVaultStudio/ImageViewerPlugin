#include "SelectionAction.h"
#include "Layer.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"

#include <util/PixelSelectionTool.h>
#include <Application.h>

using namespace hdps::util;

const auto allowedPixelSelectionTypes = PixelSelectionTypes({
    PixelSelectionType::Rectangle,
    PixelSelectionType::Brush,
    PixelSelectionType::Lasso,
    PixelSelectionType::Polygon,
    PixelSelectionType::Sample,
    PixelSelectionType::ROI
});

SelectionAction::SelectionAction(Layer& layer, QWidget* targetWidget, PixelSelectionTool& pixelSelectionTool) :
    GroupAction(&layer, "Selection"),
    _layer(layer),
    _targetWidget(targetWidget),
    _pixelSelectionAction(&layer, "Pixel Selection"),
    _pixelSelectionTool(pixelSelectionTool),
    _showRegionAction(this, "Show selected region", false, false)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));

    _showRegionAction.setVisible(false);

    addAction(&_pixelSelectionAction.getTypeAction());
    addAction(&_pixelSelectionAction.getBrushRadiusAction());
    addAction(&_pixelSelectionAction.getOverlayColorAction());
    addAction(&_pixelSelectionAction.getOverlayOpacityAction());
    addAction(&_pixelSelectionAction.getNotifyDuringSelectionAction());

    _pixelSelectionAction.initialize(targetWidget, &pixelSelectionTool, allowedPixelSelectionTypes);

    connect(&_pixelSelectionAction.getOverlayColorAction(), &ColorAction::colorChanged, &_layer, &Layer::invalidate);
    connect(&_pixelSelectionAction.getOverlayOpacityAction(), &DecimalAction::valueChanged, &_layer, &Layer::invalidate);
    connect(&_showRegionAction, &ToggleAction::toggled, &_layer, &Layer::invalidate);

    connect(&_layer.getImageViewerPlugin().getImageViewerWidget(), &ImageViewerWidget::interactionModeChanged, this, [this](const ImageViewerWidget::InteractionMode& interactionMode) {
        setEnabled(interactionMode == ImageViewerWidget::InteractionMode::Selection);
    });
}

QRect SelectionAction::getImageSelectionRectangle() const
{
    return _layer.getImageSelectionRectangle();
}
