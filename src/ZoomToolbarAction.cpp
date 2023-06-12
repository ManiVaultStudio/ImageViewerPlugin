#include "ZoomToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

using namespace hdps::util;

const float ZoomToolbarAction::zoomDeltaPercentage = 0.1f;

ZoomToolbarAction::ZoomToolbarAction(QObject* parent, const QString& title) :
    HorizontalToolbarAction(parent, title),
    _imageViewerPlugin(nullptr),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 10.0f, 1000.0f, 100.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Around Selection"),
    _exportToImageAction(this, "Export Layers")
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
}

void ZoomToolbarAction::initialize(ImageViewerPlugin* imageViewerPlugin)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;

    /*
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

    updateZoomPercentage();

    auto triggerUpdateZoomPercentageAfterAnimation = [this, updateZoomPercentage]() -> void {
        connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, this, [this, updateZoomPercentage]() {
            updateZoomPercentage();
            disconnect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, nullptr, nullptr);
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
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        getImageViewerWidget().getRenderer().setZoomRectangle(worldBoundingRectangle);
        getImageViewerWidget().update();

        triggerUpdateZoomPercentageAfterAnimation();
        });

    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        auto& _selectionModel = _imageViewerPlugin->getSelectionModel();

        const auto selectedRows = _selectionModel.selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        layer->zoomToSelection();
        layer->selectNone();

        triggerUpdateZoomPercentageAfterAnimation();
    });

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
        });

    connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::zoomRectangleChanged, this, [this, updateZoomPercentage]() {
        updateZoomPercentage();
    });
    */
}