#include "NavigationAction.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"
#include "Application.h"

#include <QHBoxLayout>

const float NavigationAction::zoomDeltaPercentage = 0.1f;

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

    _zoomOutAction.setIcon(fontAwesome.getIcon("search-minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("search-plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _panAction.setIcon(fontAwesome.getIcon("arrows-alt"));

    _zoomPercentageAction.setSuffix("%");

    connect(&_zoomOutAction, &TriggerAction::triggered, this, [this]() {
        _imageViewerWidget.getRenderer().setZoomPercentage(_imageViewerWidget.getRenderer().getZoomPercentage() - zoomDeltaPercentage);
        _imageViewerWidget.update();
    });

    connect(&_zoomPercentageAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        _imageViewerWidget.getRenderer().setZoomPercentage(0.01f * value);
        _imageViewerWidget.update();
    });

    connect(&_zoomInAction, &TriggerAction::triggered, this, [this]() {
        _imageViewerWidget.getRenderer().setZoomPercentage(_imageViewerWidget.getRenderer().getZoomPercentage() + zoomDeltaPercentage);
        _imageViewerWidget.update();
    });

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {

        // Get world bounding rectangles for all layers
        const auto worldBoundingRectangle = _imageViewerWidget.getWorldBoundingRectangle();

        // Zoom to the rectangle and render
        _imageViewerWidget.getRenderer().zoomToWorldRectangle(worldBoundingRectangle);
        _imageViewerWidget.update();
    });

    connect(&_panAction, &ToggleAction::toggled, this, [this](bool toggled) {
        _imageViewerWidget.setInteractionMode(toggled ? ImageViewerWidget::Navigation : ImageViewerWidget::LayerEditing);
    });

    const auto updateZoomPercentage = [this]() {
        _zoomPercentageAction.setValue(100.0f * _imageViewerWidget.getRenderer().getZoomPercentage());
    };

    connect(&_imageViewerWidget.getRenderer(), &Renderer::zoomPercentageChanged, this, [this, updateZoomPercentage](const float& zoomPercentage) {
        updateZoomPercentage();
    });

    updateZoomPercentage();
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
