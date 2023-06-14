#include "MainToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

#include <Application.h>
#include <util/PixelSelectionTool.h>

#include <QHBoxLayout>

using namespace hdps::util;

MainToolbarAction::MainToolbarAction(ImageViewerPlugin& imageViewerPlugin) :
    HorizontalToolbarAction(&imageViewerPlugin, "Main Toolbar"),
    _imageViewerPlugin(imageViewerPlugin),
    _panAction(this, "Pan"),
    _selectAction(this, "Select pixels"),
    _interactionModeAction(this, "Interaction Mode"),
    _rectangleSelectionAction(this, "Rectangle selection"),
    _brushSelectionAction(this, "Rectangle selection"),
    _lassoSelectionAction(this, "Lasso selection"),
    _polygonSelectionAction(this, "Polygon selection"),
    _sampleSelectionAction(this, "Sample selection"),
    _roiSelectionAction(this, "ROI selection"),
    _selectionAction(this, "Selection"),
    _exportToImageAction(this, "Export"),
    _viewSettingsAction(this, "View Settings")
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _panAction.setToolTip("Move the view");
    _selectAction.setToolTip("Select pixels");
    _exportToImageAction.setToolTip("Export to image pixels");

    _panAction.setIcon(fontAwesome.getIcon("arrows-alt"));
    _selectAction.setIcon(fontAwesome.getIcon("mouse-pointer"));
    _rectangleSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Rectangle));
    _brushSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Brush));
    _lassoSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Lasso));
    _polygonSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Polygon));
    _sampleSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Sample));
    _roiSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::ROI));
    _exportToImageAction.setIcon(fontAwesome.getIcon("camera"));
    
    _interactionModeAction.setIcon(fontAwesome.getIcon("hand-sparkles"));
    _interactionModeAction.setToolTip("Interaction type");
    _interactionModeAction.addAction(&_panAction, ToggleAction::PushButtonIcon);
    _interactionModeAction.addAction(&_selectAction, ToggleAction::PushButtonIcon);

    _selectionAction.setIcon(fontAwesome.getIcon("mouse-pointer"));
    _selectionAction.setToolTip("Selection type");
    _selectionAction.addAction(&_rectangleSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_brushSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_lassoSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_polygonSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_sampleSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_roiSelectionAction, ToggleAction::PushButtonIcon);

    addAction(&_interactionModeAction, 2);
    addAction(&_selectionAction, 1);

    getImageViewerWidget().addAction(&_panAction);
    getImageViewerWidget().addAction(&_selectAction);
    getImageViewerWidget().addAction(&_exportToImageAction);

    connect(&_panAction, &ToggleAction::toggled, this, [this](bool toggled) {
        getImageViewerWidget().setInteractionMode(toggled ? ImageViewerWidget::Navigation : ImageViewerWidget::Selection);
    });

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
    });

    const auto updateInteractionActions = [this]() -> void {
        const auto inSelectionMode  = getImageViewerWidget().getInteractionMode() == ImageViewerWidget::InteractionMode::Selection;
        const auto maySelect        = inSelectionMode && !_imageViewerPlugin.getSelectionModel().selectedRows().isEmpty();

        // Enable/disable selection type actions
        _rectangleSelectionAction.setEnabled(maySelect);
        _brushSelectionAction.setEnabled(maySelect);
        _lassoSelectionAction.setEnabled(maySelect);
        _polygonSelectionAction.setEnabled(maySelect);
        _sampleSelectionAction.setEnabled(maySelect);
    };

    connect(&getImageViewerWidget(), &ImageViewerWidget::interactionModeChanged, this, updateInteractionActions);
    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, updateInteractionActions);

    updateInteractionActions();

    setupInteraction();
}

ImageViewerWidget& MainToolbarAction::getImageViewerWidget()
{
    return _imageViewerPlugin.getImageViewerWidget();
}

void MainToolbarAction::setupInteraction()
{
    const auto getSelectedLayer = [this]() -> Layer* {

        // Get selected row from selection model
        const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

        // Only accept one selected layer at a time
        if (selectedRows.isEmpty())
            return nullptr;

        return static_cast<Layer*>(selectedRows.first().internalPointer());
    };

    connect(&_rectangleSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getRectangleAction().setChecked(toggled);
    });

    connect(&_brushSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getBrushAction().setChecked(toggled);
    });

    connect(&_lassoSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getLassoAction().setChecked(toggled);
    });

    connect(&_polygonSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getPolygonAction().setChecked(toggled);
    });

    connect(&_sampleSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getSampleAction().setChecked(toggled);
    });

    connect(&_roiSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPixelSelectionAction().getRoiAction().setChecked(toggled);
    });

    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& newSelection, const QItemSelection& oldSelection) {

        // Process deselected layers
        if (!oldSelection.indexes().isEmpty()) {

            // Get pointer to layer that was deselected
            auto layer = static_cast<Layer*>(oldSelection.indexes().first().internalPointer());

            // Get reference to the layer pixel selection action
            auto& pixelSelectionAction = layer->getSelectionAction().getPixelSelectionAction();

            // Disconnect
            disconnect(&pixelSelectionAction.getRectangleAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&pixelSelectionAction.getBrushAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&pixelSelectionAction.getLassoAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&pixelSelectionAction.getPolygonAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&pixelSelectionAction.getSampleAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&pixelSelectionAction.getRoiAction(), &ToggleAction::toggled, this, nullptr);
        }

        // Process selected layers
        if (!newSelection.indexes().isEmpty()) {

            // Get pointer to layer that was selected
            auto layer = static_cast<Layer*>(newSelection.indexes().first().internalPointer());

            // Get reference to the layer pixel selection action
            auto& pixelSelectionAction = layer->getSelectionAction().getPixelSelectionAction();

            // Update the check state of the selection actions
            const auto updateSelectionActions = [this, &pixelSelectionAction]() -> void {
                _rectangleSelectionAction.setChecked(pixelSelectionAction.getRectangleAction().isChecked());
                _brushSelectionAction.setChecked(pixelSelectionAction.getBrushAction().isChecked());
                _lassoSelectionAction.setChecked(pixelSelectionAction.getLassoAction().isChecked());
                _polygonSelectionAction.setChecked(pixelSelectionAction.getPolygonAction().isChecked());
                _sampleSelectionAction.setChecked(pixelSelectionAction.getSampleAction().isChecked());
                _roiSelectionAction.setChecked(pixelSelectionAction.getRoiAction().isChecked());
            };

            connect(&pixelSelectionAction.getRectangleAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&pixelSelectionAction.getBrushAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&pixelSelectionAction.getLassoAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&pixelSelectionAction.getPolygonAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&pixelSelectionAction.getSampleAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&pixelSelectionAction.getRoiAction(), &ToggleAction::toggled, this, updateSelectionActions);

            // Do an initial update when the layer is selected
            updateSelectionActions();
        }
    });
}