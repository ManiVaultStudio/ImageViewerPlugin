#include "NavigationAction.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"
#include "Application.h"

#include <QHBoxLayout>

const float NavigationAction::zoomAmount = 0.05f;

NavigationAction::NavigationAction(ImageViewerWidget& imageViewerWidget) :
    WidgetAction(&imageViewerWidget),
    _imageViewerWidget(imageViewerWidget),
    _zoomOutAction(this, ""),
    _zoomPercentageAction(this, "Zoom percentage", 0.0f, 500.0f, 100.0f, 100.0f, 1),
    _zoomInAction(this, ""),
    _zoomExtentsAction(this, ""),
    _panAction(this, "")
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _zoomOutAction.setToolTip("Zoom out by 5%");
    _zoomPercentageAction.setToolTip("Zoom in/out");
    _zoomInAction.setToolTip("Zoom in by 5%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers");
    _panAction.setToolTip("Move the view");

    _zoomOutAction.setIcon(fontAwesome.getIcon("minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _panAction.setIcon(fontAwesome.getIcon("arrows-alt"));

    _zoomPercentageAction.setSuffix("%");

    const auto zoomByFactor = [this](const float& factor) {
        _imageViewerWidget.getRenderer().zoomBy(factor);
        _imageViewerWidget.update();
    };

    connect(&_zoomOutAction, &TriggerAction::triggered, this, [zoomByFactor]() {
        zoomByFactor(1.0f - zoomAmount);
    });

    connect(&_zoomInAction, &TriggerAction::triggered, this, [zoomByFactor]() {
        zoomByFactor(1.0f + zoomAmount);
    });

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {

        // Get world bounding rectangles for all layers
        const auto worldBoundingRectangle = _imageViewerWidget.getLayersModel().getWorldBoundingRectangle();

        // Zoom to the rectangle and render
        _imageViewerWidget.getRenderer().zoomToWorldRectangle(worldBoundingRectangle, ImageViewerWidget::getZoomMargin());
        _imageViewerWidget.update();
    });

    connect(&_panAction, &ToggleAction::toggled, this, [this](bool toggled) {
        _imageViewerWidget.setInteractionMode(toggled ? ImageViewerWidget::Navigation : ImageViewerWidget::LayerEditing);
    });

    connect(&_imageViewerWidget.getRenderer(), &Renderer::zoomLevelChanged, this, [this](const float& zoom) {

        // Get screen bounding rectangles for all layers
        const auto screenBoundingRectangle   = _imageViewerWidget.getRenderer().getScreenBoundingRectangle(_imageViewerWidget.getLayersModel().getWorldBoundingRectangle());

        const auto viewerSize       = _imageViewerWidget.size();
        const auto totalMargins     = 2 * ImageViewerWidget::getZoomMargin();
        const auto factorX          = static_cast<float>(std::abs(screenBoundingRectangle.width())) / (viewerSize.width() - totalMargins);
        const auto factorY          = static_cast<float>(std::abs(screenBoundingRectangle.height())) / (viewerSize.height() - totalMargins);
        const auto zoomPercentage   = 100.0f * (factorX > factorY ? factorX : factorY);

        qDebug() << factorX << factorY;

        _zoomPercentageAction.setValue(zoomPercentage);
    });
}

ImageViewerWidget& NavigationAction::getImageViewerWidget()
{
    return _imageViewerWidget;
}

NavigationAction::Widget::Widget(QWidget* parent, NavigationAction* navigationAction, const WidgetActionWidget::State& state) :
    WidgetActionWidget(parent, navigationAction, state)
{
    setAutoFillBackground(true);

    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(4);

    layout->addWidget(navigationAction->getZoomOutAction().createWidget(this));
    layout->addWidget(navigationAction->getZoomPercentageAction().createWidget(this));
    layout->addWidget(navigationAction->getZoomInAction().createWidget(this));
    layout->addWidget(navigationAction->getZoomExtentsAction().createWidget(this));
    layout->addWidget(navigationAction->getPanAction().createPushButtonWidget(this));
    layout->addStretch(1);

    setLayout(layout);
}
