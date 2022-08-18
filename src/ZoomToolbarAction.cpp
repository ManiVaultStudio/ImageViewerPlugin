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
    _zoomSelectionAction(this, "Zoom around selection"),
    _exportToImageAction(this, "Export layers to image")
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers (z)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the selection (d)");
    _exportToImageAction.setToolTip("Export to image pixels");

    _zoomOutAction.setIcon(fontAwesome.getIcon("search-minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("search-plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _zoomSelectionAction.setIcon(fontAwesome.getIcon("search-plus"));
    _exportToImageAction.setIcon(fontAwesome.getIcon("camera"));
    
    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _zoomSelectionAction.setShortcut(QKeySequence("d"));
    _exportToImageAction.setShortcut(QKeySequence("e"));

    _zoomPercentageAction.setSuffix("%");
    _zoomPercentageAction.setDefaultWidgetFlags(IntegralAction::Slider);
    _zoomPercentageAction.setUpdateDuringDrag(false);

    getImageViewerWidget().addAction(&_zoomOutAction);
    getImageViewerWidget().addAction(&_zoomInAction);
    getImageViewerWidget().addAction(&_zoomExtentsAction);
    getImageViewerWidget().addAction(&_zoomSelectionAction); 
    getImageViewerWidget().addAction(&_exportToImageAction);

    const auto updateZoomPercentage = [this]() -> void {
        const auto zoomPercentage = 100.0f * getImageViewerWidget().getRenderer().getZoomPercentage();

        _zoomOutAction.setEnabled(zoomPercentage > _zoomPercentageAction.getMinimum());
        _zoomPercentageAction.setValue(zoomPercentage);
        _zoomInAction.setEnabled(zoomPercentage < _zoomPercentageAction.getMaximum());

    };

    auto triggerUpdateZoomPercentageAfterAnimation = [this, updateZoomPercentage]() -> void {
        // what until the animation is over - only then are the correct view ROI values set internally correctly
        connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, this, [this, updateZoomPercentage]() {

            updateZoomPercentage();

            // disconnect this lambda again
            QObject::disconnect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, nullptr, nullptr);
            });
    };

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

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {

        // Get world bounding rectangles for all layers
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        // Zoom to the rectangle and render
        getImageViewerWidget().getRenderer().setZoomRectangle(worldBoundingRectangle);
        getImageViewerWidget().update();

        triggerUpdateZoomPercentageAfterAnimation();
        });

    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        auto& _selectionModel = _imageViewerPlugin.getSelectionModel();

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Zoom to selection, deselect points for interactive hsne update to work
        layer->zoomToSelection();
        layer->selectNone();

        // Inform interactive hsne 
        triggerUpdateZoomPercentageAfterAnimation();
        });

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
        });

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
    layout->setContentsMargins(4, 4, 4, 4);

    layout->addStretch(1);
    layout->addWidget(zoomToolbarAction->getZoomOutAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(zoomToolbarAction->getZoomPercentageAction().createWidget(this, DecimalAction::Slider));
    layout->addWidget(zoomToolbarAction->getZoomInAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(zoomToolbarAction->getZoomExtentsAction().createWidget(this, TriggerAction::Icon));
    layout->addWidget(zoomToolbarAction->getZoomSelectionAction().createWidget(this, TriggerAction::Icon));
    //layout->addWidget(navigationAction->getExportToImageAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addStretch(1);

    setLayout(layout);
}
