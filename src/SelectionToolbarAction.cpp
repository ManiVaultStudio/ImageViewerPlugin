#include "SelectionToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

#include <Application.h>
#include <util/PixelSelectionTool.h>

#include <QHBoxLayout>

using namespace mv::util;

SelectionToolbarAction::SelectionToolbarAction(ImageViewerPlugin& imageViewerPlugin) :
    HorizontalToolbarAction(&imageViewerPlugin, "Main Toolbar"),
    _imageViewerPlugin(imageViewerPlugin),
    _rectangleSelectionAction(this, "Rectangle selection"),
    _brushSelectionAction(this, "Rectangle selection"),
    _lassoSelectionAction(this, "Lasso selection"),
    _polygonSelectionAction(this, "Polygon selection"),
    _sampleSelectionAction(this, "Sample selection"),
    _roiSelectionAction(this, "ROI selection"),
    _selectionAction(this, "Selection"),
    _modifierAction(this, "Modifier", { "Replace", "Add", "Subtract" })
{
    _rectangleSelectionAction.setEnabled(false);
    _brushSelectionAction.setEnabled(false);
    _lassoSelectionAction.setEnabled(false);
    _polygonSelectionAction.setEnabled(false);
    _sampleSelectionAction.setEnabled(false);
    _roiSelectionAction.setEnabled(false);
    _modifierAction.setEnabled(false);

    _rectangleSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Rectangle));
    _brushSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Brush));
    _lassoSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Lasso));
    _polygonSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Polygon));
    _sampleSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::Sample));
    _roiSelectionAction.setIcon(getPixelSelectionTypeIcon(PixelSelectionType::ROI));

    _selectionAction.setIconByName("mouse-pointer");
    _selectionAction.setToolTip("Selection type");
    _selectionAction.setShowLabels(false);

    _selectionAction.addAction(&_rectangleSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_brushSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_lassoSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_polygonSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_sampleSelectionAction, ToggleAction::PushButtonIcon);
    _selectionAction.addAction(&_modifierAction);

    _modifierAction.setToolTip("Determines how to combined with the current selection");

    addAction(&_selectionAction, 1, GroupAction::Horizontal);

    setupInteraction();
}

ImageViewerWidget& SelectionToolbarAction::getImageViewerWidget()
{
    return _imageViewerPlugin.getImageViewerWidget();
}

void SelectionToolbarAction::setupInteraction()
{
    const auto unlinkToggleAction = [this](ToggleAction* toggleAction) -> void {
        disconnect(toggleAction, &ToggleAction::toggled, this, nullptr);
        disconnect(toggleAction, &QAction::enabledChanged, this, nullptr);
    };

    const auto linkToggleActions = [this](ToggleAction* toggleActionSource, ToggleAction* toggleActionTarget) -> void {
        getImageViewerWidget().addAction(toggleActionSource);

        toggleActionSource->setChecked(toggleActionTarget->isChecked());

        connect(toggleActionSource, &ToggleAction::toggled, this, [this, toggleActionTarget](bool toggled) {
            toggleActionTarget->setChecked(toggled);
        });

        connect(toggleActionTarget, &ToggleAction::toggled, this, [this, toggleActionSource](bool toggled) {
            toggleActionSource->setChecked(toggled);
        });

        const auto updateReadOnly = [toggleActionSource, toggleActionTarget]() -> void {
            toggleActionSource->setEnabled(toggleActionTarget->isEnabled());
        };

        updateReadOnly();

        connect(toggleActionTarget, &QAction::enabledChanged, this, updateReadOnly);
    };

    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this, unlinkToggleAction, linkToggleActions](const QItemSelection& newSelection, const QItemSelection& oldSelection) {
        if (!oldSelection.indexes().isEmpty()) {
            auto layer = _imageViewerPlugin.getLayersModel().getLayerFromIndex(oldSelection.indexes().first());

            auto& pixelSelectionAction = layer->getSelectionAction().getPixelSelectionAction();

            unlinkToggleAction(&pixelSelectionAction.getRectangleAction());
            unlinkToggleAction(&pixelSelectionAction.getBrushAction());
            unlinkToggleAction(&pixelSelectionAction.getLassoAction());
            unlinkToggleAction(&pixelSelectionAction.getPolygonAction());
            unlinkToggleAction(&pixelSelectionAction.getSampleAction());
            unlinkToggleAction(&pixelSelectionAction.getRoiAction());
            
            disconnect(&_modifierAction, &OptionAction::currentIndexChanged, this, nullptr);
            disconnect(&pixelSelectionAction.getModifierAction(), &OptionAction::currentIndexChanged, this, nullptr);
            disconnect(&pixelSelectionAction.getModifierAction(), &QAction::enabledChanged, this, nullptr);
        }

        if (!newSelection.indexes().isEmpty()) {
            auto layer = _imageViewerPlugin.getLayersModel().getLayerFromIndex(newSelection.indexes().first());

            auto& pixelSelectionAction = layer->getSelectionAction().getPixelSelectionAction();

            linkToggleActions(&_rectangleSelectionAction, &pixelSelectionAction.getRectangleAction());
            linkToggleActions(&_brushSelectionAction, &pixelSelectionAction.getBrushAction());
            linkToggleActions(&_lassoSelectionAction, &pixelSelectionAction.getLassoAction());
            linkToggleActions(&_polygonSelectionAction, &pixelSelectionAction.getPolygonAction());
            linkToggleActions(&_sampleSelectionAction, &pixelSelectionAction.getSampleAction());
            linkToggleActions(&_roiSelectionAction, &pixelSelectionAction.getRoiAction());
            
            _modifierAction.setCurrentIndex(pixelSelectionAction.getModifierAction().getCurrentIndex());

            connect(&_modifierAction, &OptionAction::currentIndexChanged, this, [this, &pixelSelectionAction](const std::int32_t& currentIndex) {
                pixelSelectionAction.getModifierAction().setCurrentIndex(currentIndex);
            });

            connect(&pixelSelectionAction.getModifierAction(), &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
                _modifierAction.setCurrentIndex(currentIndex);
            });

            const auto updateReadOnly = [this, &pixelSelectionAction]() -> void {
                _modifierAction.setEnabled(pixelSelectionAction.getModifierAction().isEnabled());
            };

            updateReadOnly();

            connect(&pixelSelectionAction.getModifierAction(), &QAction::enabledChanged, this, updateReadOnly);
        }
    });
}