#include "MainToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"
#include "Application.h"

#include "util/PixelSelectionTool.h"

#include <QHBoxLayout>

using namespace hdps::util;

MainToolbarAction::MainToolbarAction(ImageViewerPlugin& imageViewerPlugin) :
    WidgetAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _panAction(this, "Pan"),
    _selectAction(this, "Select pixels"),
    _rectangleSelectionAction(this, "Rectangle selection"),
    _brushSelectionAction(this, "Rectangle selection"),
    _lassoSelectionAction(this, "Lasso selection"),
    _polygonSelectionAction(this, "Polygon selection"),
    _sampleSelectionAction(this, "Sample selection"),
    _subsetAction(imageViewerPlugin),
    _exportToImageAction(this, ""),
    _interactionModeActionGroup(this),
    _globalViewSettingsAction(imageViewerPlugin)
{
    setText("Navigation");

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _panAction.setToolTip("Move the view");
    _selectAction.setToolTip("Select pixels");
    _subsetAction.setToolTip("Create subset from selection");
    _exportToImageAction.setToolTip("Export to image pixels");

    _panAction.setIcon(fontAwesome.getIcon("arrows-alt"));
    _selectAction.setIcon(fontAwesome.getIcon("mouse-pointer"));
    _rectangleSelectionAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Rectangle));
    _brushSelectionAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Brush));
    _lassoSelectionAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Lasso));
    _polygonSelectionAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Polygon));
    _sampleSelectionAction.setIcon(PixelSelectionTool::getIcon(PixelSelectionType::Sample));
    _exportToImageAction.setIcon(fontAwesome.getIcon("camera"));
    
    _interactionModeActionGroup.addAction(&_panAction);
    _interactionModeActionGroup.addAction(&_selectAction);

    getImageViewerWidget().addAction(&_panAction);
    getImageViewerWidget().addAction(&_selectAction);
    getImageViewerWidget().addAction(&_exportToImageAction);
    getImageViewerWidget().addAction(&_subsetAction);

    connect(&_panAction, &ToggleAction::toggled, this, [this](bool toggled) {
        getImageViewerWidget().setInteractionMode(toggled ? ImageViewerWidget::Navigation : ImageViewerWidget::Selection);
    });

    connect(&_exportToImageAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().exportToImage();
    });

    const auto updateInteractionActions = [this]() -> void {
        const auto inSelectionMode = getImageViewerWidget().getInteractionMode() == ImageViewerWidget::InteractionMode::Selection;

        _panAction.setChecked(!inSelectionMode);
        _selectAction.setChecked(inSelectionMode);

        // Determine whether selection may take place
        const auto maySelect = inSelectionMode && !_imageViewerPlugin.getSelectionModel().selectedRows().isEmpty();

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
            selectedLayer->getSelectionAction().getRectangleAction().setChecked(toggled);
    });

    connect(&_brushSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getBrushAction().setChecked(toggled);
    });

    connect(&_lassoSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getLassoAction().setChecked(toggled);
    });

    connect(&_polygonSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getPolygonAction().setChecked(toggled);
    });

    connect(&_sampleSelectionAction, &ToggleAction::toggled, this, [this, getSelectedLayer](bool toggled) {
        auto selectedLayer = getSelectedLayer();

        if (selectedLayer)
            selectedLayer->getSelectionAction().getSampleAction().setChecked(toggled);
    });

    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& newSelection, const QItemSelection& oldSelection) {

        // Process deselected layers
        if (!oldSelection.indexes().isEmpty()) {

            // Get pointer to layer that was deselected
            auto layer = static_cast<Layer*>(oldSelection.indexes().first().internalPointer());

            // Get reference to the layer selection action
            auto& selectionAction = layer->getSelectionAction();

            // Disconnect
            disconnect(&selectionAction.getRectangleAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&selectionAction.getBrushAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&selectionAction.getLassoAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&selectionAction.getPolygonAction(), &ToggleAction::toggled, this, nullptr);
            disconnect(&selectionAction.getSampleAction(), &ToggleAction::toggled, this, nullptr);
        }

        // Process selected layers
        if (!newSelection.indexes().isEmpty()) {

            // Get pointer to layer that was selected
            auto layer = static_cast<Layer*>(newSelection.indexes().first().internalPointer());

            // Get reference to the layer selection action
            auto& selectionAction = layer->getSelectionAction();

            // Update the check state of the selection actions
            const auto updateSelectionActions = [this, &selectionAction]() -> void {
                _rectangleSelectionAction.setChecked(selectionAction.getRectangleAction().isChecked());
                _brushSelectionAction.setChecked(selectionAction.getBrushAction().isChecked());
                _lassoSelectionAction.setChecked(selectionAction.getLassoAction().isChecked());
                _polygonSelectionAction.setChecked(selectionAction.getPolygonAction().isChecked());
                _sampleSelectionAction.setChecked(selectionAction.getSampleAction().isChecked());
            };

            connect(&selectionAction.getRectangleAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&selectionAction.getBrushAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&selectionAction.getLassoAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&selectionAction.getPolygonAction(), &ToggleAction::toggled, this, updateSelectionActions);
            connect(&selectionAction.getSampleAction(), &ToggleAction::toggled, this, updateSelectionActions);

            // Do an initial update when the layer is selected
            updateSelectionActions();
        }
    });
}

MainToolbarAction::Widget::Widget(QWidget* parent, MainToolbarAction* interactionAction) :
    WidgetActionWidget(parent, interactionAction)
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

    layout->addWidget(interactionAction->getPanAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(interactionAction->getSelectAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(getDivider());
    layout->addWidget(interactionAction->getRectangleSelectionAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(interactionAction->getBrushSelectionAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(interactionAction->getLassoSelectionAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(interactionAction->getPolygonSelectionAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(interactionAction->getSampleSelectionAction().createWidget(this, ToggleAction::PushButtonIcon));
    layout->addWidget(getDivider());
    layout->addWidget(interactionAction->getSubsetAction().createCollapsedWidget(this));
    layout->addStretch(1);
    layout->addWidget(interactionAction->getGlobalViewSettingsAction().createCollapsedWidget(this));

    setLayout(layout);
}