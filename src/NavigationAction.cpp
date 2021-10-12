#include "NavigationAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"
#include "Application.h"

#include <QHBoxLayout>

const float NavigationAction::zoomDeltaPercentage = 0.1f;

NavigationAction::NavigationAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _zoomOutAction(this, ""),
    _zoomPercentageAction(this, "Zoom percentage", 1.0f, 1000.0f, 100.0f, 100.0f, 1),
    _zoomInAction(this, ""),
    _zoomExtentsAction(this, "Zoom all"),
    _panAction(this, "Pan"),
    _selectAction(this, "Select pixels"),
    _subsetAction(imageViewerPlugin),
    _exportToImageAction(this, ""),
    _interactionModeActionGroup(this)
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _zoomOutAction.setToolTip("Zoom out by 5% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 5%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers (z)");
    _panAction.setToolTip("Move the view");
    _selectAction.setToolTip("Select pixels");
    _subsetAction.setToolTip("Create subset from selection");
    _exportToImageAction.setToolTip("Export to image pixels");

    _zoomOutAction.setIcon(fontAwesome.getIcon("search-minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("search-plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _panAction.setIcon(fontAwesome.getIcon("arrows-alt"));
    _selectAction.setIcon(fontAwesome.getIcon("mouse-pointer"));
    _exportToImageAction.setIcon(fontAwesome.getIcon("camera"));
    
    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _exportToImageAction.setShortcut(QKeySequence("e"));
    //_panAction.setShortcut(QKeySequence("R"));
    //_selectAction.setShortcut(QKeySequence("R"));
    _subsetAction.setShortcut(QKeySequence("S"));

    _zoomPercentageAction.setSuffix("%");

    _interactionModeActionGroup.addAction(&_panAction);
    _interactionModeActionGroup.addAction(&_selectAction);

    getImageViewerWidget().addAction(&_zoomOutAction);
    getImageViewerWidget().addAction(&_zoomInAction);
    getImageViewerWidget().addAction(&_zoomExtentsAction);
    getImageViewerWidget().addAction(&_panAction);
    getImageViewerWidget().addAction(&_selectAction);
    getImageViewerWidget().addAction(&_exportToImageAction);
    getImageViewerWidget().addAction(&_subsetAction);

    connect(&_zoomOutAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() - zoomDeltaPercentage);
        getImageViewerWidget().update();
    });

    connect(&_zoomPercentageAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        getImageViewerWidget().getRenderer().setZoomPercentage(0.01f * value);
        getImageViewerWidget().update();
    });

    connect(&_zoomInAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() + zoomDeltaPercentage);
        getImageViewerWidget().update();
    });

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this]() {

        // Get world bounding rectangles for all layers
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        // Zoom to the rectangle and render
        getImageViewerWidget().getRenderer().setZoomRectangle(worldBoundingRectangle);
        getImageViewerWidget().update();
    });

    connect(&_panAction, &ToggleAction::toggled, this, [this](bool toggled) {
        getImageViewerWidget().setInteractionMode(toggled ? ImageViewerWidget::Navigation : ImageViewerWidget::Selection);
    });

    connect(&_selectAction, &ToggleAction::toggled, this, [this](bool toggled) {
        getImageViewerWidget().setInteractionMode(toggled ? ImageViewerWidget::Selection : ImageViewerWidget::Navigation);
    });

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
    });

    const auto updateZoomPercentage = [this]() {
        _zoomPercentageAction.setValue(100.0f * getImageViewerWidget().getRenderer().getZoomPercentage());
    };

    connect(&getImageViewerWidget().getRenderer(), &Renderer::zoomRectangleChanged, this, [this, updateZoomPercentage]() {
        updateZoomPercentage();
    });

    connect(&getImageViewerWidget(), &ImageViewerWidget::interactionModeChanged, this, [this](const ImageViewerWidget::InteractionMode& interactionMode) {
        _panAction.setChecked(interactionMode == ImageViewerWidget::InteractionMode::Navigation);
        _selectAction.setChecked(interactionMode == ImageViewerWidget::InteractionMode::Selection);
    });

    updateZoomPercentage();
}

ImageViewerWidget& NavigationAction::getImageViewerWidget()
{
    return *_imageViewerPlugin.getImageViewerWidget();
}

NavigationAction::Widget::Widget(QWidget* parent, NavigationAction* navigationAction) :
    WidgetActionWidget(parent, navigationAction)
{
    setAutoFillBackground(true);

    const auto getDivider = []() -> QFrame* {
        auto divider = new QFrame();

        divider->setFrameShape(QFrame::VLine);
        divider->setFrameShadow(QFrame::Sunken);

        return divider;
    };

    auto layout = new QHBoxLayout();

    layout->setSpacing(3);
    layout->setMargin(4);

    layout->addStretch(1);
    layout->addWidget(navigationAction->getPanAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(navigationAction->getSelectAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(getDivider());
    layout->addWidget(navigationAction->getZoomOutAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(navigationAction->getZoomPercentageAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(navigationAction->getZoomInAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(navigationAction->getZoomExtentsAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(getDivider());
    layout->addWidget(navigationAction->getSubsetAction().createWidget(this, ToggleAction::PushButtonIcon));
    //layout->addWidget(navigationAction->getExportToImageAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addStretch(1);

    setLayout(layout);
}
