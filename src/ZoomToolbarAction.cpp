#include "ZoomToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

#include <Application.h>
#include <util/PixelSelectionTool.h>

#include <QHBoxLayout>

using namespace hdps::util;

const float ZoomToolbarAction::zoomDeltaPercentage = 0.1f;

ZoomToolbarAction::ZoomToolbarAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _zoomOutAction(this, ""),
    _zoomPercentageAction(this, "Zoom percentage", 10.0f, 1000.0f, 100.0f, 100.0f, 1),
    _zoomInAction(this, ""),
    _zoomExtentsAction(this, "Zoom all"),
    _exportToImageAction(this, "Export layers to image")
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _zoomOutAction.setToolTip("Zoom out by 5% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 5%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers (z)");
    _exportToImageAction.setToolTip("Export to image pixels");

    _zoomOutAction.setIcon(fontAwesome.getIcon("search-minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("search-plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _exportToImageAction.setIcon(fontAwesome.getIcon("camera"));
    
    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _exportToImageAction.setShortcut(QKeySequence("e"));

    _zoomPercentageAction.setSuffix("%");

    getImageViewerWidget().addAction(&_zoomOutAction);
    getImageViewerWidget().addAction(&_zoomInAction);
    getImageViewerWidget().addAction(&_zoomExtentsAction);
    getImageViewerWidget().addAction(&_exportToImageAction);

    connect(&_zoomOutAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() - zoomDeltaPercentage);
        getImageViewerWidget().update();
    });

    connect(&_zoomPercentageAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        //getImageViewerWidget().getRenderer().setZoomPercentage(0.01f * value);
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

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
    });

    const auto updateZoomPercentage = [this]() {
        const auto zoomPercentage = 100.0f * getImageViewerWidget().getRenderer().getZoomPercentage();

        _zoomOutAction.setEnabled(zoomPercentage > _zoomPercentageAction.getMinimum());
        _zoomPercentageAction.setValue(zoomPercentage);
        _zoomInAction.setEnabled(zoomPercentage < _zoomPercentageAction.getMaximum());
    };

    connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::zoomRectangleChanged, this, [this, updateZoomPercentage]() {
        updateZoomPercentage();
    });

    updateZoomPercentage();
}

ImageViewerWidget& ZoomToolbarAction::getImageViewerWidget()
{
    return _imageViewerPlugin.getImageViewerWidget();
}

ZoomToolbarAction::Widget::Widget(QWidget* parent, ZoomToolbarAction* zoomToolbarAction) :
    WidgetActionWidget(parent, zoomToolbarAction)
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
    layout->addWidget(zoomToolbarAction->getZoomOutAction().createWidget(this, TriggerAction::Icon));
    //layout->addWidget(zoomToolbarAction->getZoomPercentageAction().createWidget(this, DecimalAction::SpinBox | DecimalAction::Slider));
    layout->addWidget(zoomToolbarAction->getZoomInAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(zoomToolbarAction->getZoomExtentsAction().createWidget(this, TriggerAction::Icon));
    //layout->addWidget(navigationAction->getExportToImageAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addStretch(1);

    setLayout(layout);
}
