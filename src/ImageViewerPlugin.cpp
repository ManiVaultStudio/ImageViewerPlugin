#include "ImageViewerPlugin.h"
#include "SelectionAction.h"
#include "Layer.h"
#include "ConvertToImagesDatasetDialog.h"

#include <ImageData/Images.h>
#include <ImageData/ImageData.h>
#include <ClusterData.h>
#include <util/Exception.h>

#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QSplitter>
#include <QMimeData>
#include <QMenu>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin(hdps::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _model(this),
    _selectionModel(&_model),
    _splitter(Qt::Horizontal, &getWidget()),
    _imageViewerWidget(*this),
    _dropWidget(&_imageViewerWidget),
    _mainToolbarAction(*this),
    _zoomToolbarAction(*this),
    _settingsAction(*this)
{
    setObjectName("Images");

    getWidget().setContextMenuPolicy(Qt::CustomContextMenu);
    getWidget().setFocusPolicy(Qt::ClickFocus);
}

void ImageViewerPlugin::init()
{
    // Create main layout for view and editing
    auto mainLayout = new QHBoxLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    getWidget().setLayout(mainLayout);

    // Create main left widget
    auto mainWidget = new QWidget();

    // Create main widget layout
    auto mainWidgetLayout = new QVBoxLayout();

    // Configure main layout
    mainWidgetLayout->setContentsMargins(0, 0, 0, 0);
    mainWidgetLayout->setSpacing(0);

    // And add the toolbar, image viewer widget
    mainWidgetLayout->addWidget(_mainToolbarAction.createWidget(&getWidget()));
    mainWidgetLayout->addWidget(&_imageViewerWidget, 1);
    mainWidgetLayout->addWidget(_zoomToolbarAction.createWidget(&getWidget()));

    // Apply layout to main widget
    mainWidget->setLayout(mainWidgetLayout);

    // Add viewer widget and settings panel to the splitter
    _splitter.addWidget(mainWidget);
    _splitter.addWidget(_settingsAction.createWidget(&getWidget()));

    // Configure splitter
    _splitter.setStretchFactor(0, 1);
    _splitter.setStretchFactor(1, 0);
    _splitter.setCollapsible(1, true);

    // Add splitter to the main layout
    mainLayout->addWidget(&_splitter);

    // Provide a hint on how to load data
    _dropWidget.setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(&getWidget(), "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    // Establish droppable regions
    _dropWidget.initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText         = mimeData->text();
        const auto tokens           = mimeText.split("\n");
        const auto datasetGuiName   = tokens[0];
        const auto datasetId        = tokens[1];
        const auto dataType         = hdps::DataType(tokens[2]);
        const auto dataTypes        = hdps::DataTypes({ ImageType, PointType, ClusterType });

        // Get reference to the drop dataset
        auto dataset = _core->requestDataset(datasetId);

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", "exclamation-circle", false);

        if (dataType == ImageType) {
            dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetGuiName), "images", true, [this, datasetGuiName, dataset]() {
                try
                {
                    addDataset(dataset);
                }
                catch (std::exception& e)
                {
                    exceptionMessageBox(QString("Unable to load '%1'").arg(datasetGuiName), e);
                }
                catch (...) {
                    exceptionMessageBox(QString("Unable to load '%1'").arg(datasetGuiName));
                }
            });
        }

        if (dataType == PointType) {
            dropRegions << new DropWidget::DropRegion(this, "Points", QString("Convert %1 to image layer").arg(datasetGuiName), "braille", true, [this, dataset]() {

                // Convert the points dataset to an images dataset and add as a layer
                immigrateDataset(dataset);
            });
        }

        if (dataType == ClusterType) {
            dropRegions << new DropWidget::DropRegion(this, "Clusters", QString("Convert %1 to image layer").arg(datasetGuiName), "th-large", true, [this, dataset]() {

                // Convert the points dataset to an images dataset and add as a layer
                immigrateDataset(dataset);
            });
        }

        return dropRegions;
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::pixelSelectionStarted, this, [this]() {

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Publish the selection
        layer->startSelection();
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::mousePositionsChanged, this, [this](const QVector<QPoint>& mousePositions) {

        // No point in computing selection when there are no mouse positions
        if (mousePositions.count() == 0)
            return;

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Compute the layer selection
        layer->computeSelection(mousePositions);

        // Get reference to the selection action
        auto& selectionAction = layer->getSelectionAction();

        // Establish whether the selection type is sample
        const auto isSampleSelection = layer->getSelectionAction().getPixelSelectionAction().getTypeAction().getCurrentIndex() == static_cast<std::int32_t>(PixelSelectionType::Sample);

        // Publish the selection
        if (selectionAction.getPixelSelectionAction().getNotifyDuringSelectionAction().isChecked() || isSampleSelection)
            layer->publishSelection();

        // Reset the off-screen selection buffer in the case of sample selection
        if (isSampleSelection)
            layer->resetSelectionBuffer();
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::pixelSelectionEnded, this, [this]() {

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Publish the selection if notifications during selection are turned on
        layer->publishSelection();

        // Reset the off-screen selection buffer
        layer->resetSelectionBuffer();
    });

    const auto layersInsertedRemovedChanged = [this]() {
        _dropWidget.setShowDropIndicator(_model.rowCount() == 0);

        // Establish the number of visible layers
        const auto hasVisibleLayers = _model.rowCount() == 0 ? false : !_model.match(_model.index(0, LayersModel::Visible), Qt::EditRole, true, -1).isEmpty();

        // Enabled/disable navigation tool bar
        _mainToolbarAction.setEnabled(hasVisibleLayers);
        _zoomToolbarAction.setEnabled(hasVisibleLayers);
    };

    // Enable/disable the navigation action when rows are inserted/removed
    connect(&_model, &LayersModel::rowsInserted, this, layersInsertedRemovedChanged);
    connect(&_model, &LayersModel::rowsRemoved, this, layersInsertedRemovedChanged);
    connect(&_model, &LayersModel::dataChanged, this, layersInsertedRemovedChanged);

    // Initially enable/disable the navigation action
    layersInsertedRemovedChanged();

    // Change the window title when the layer selection or layer name changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewerPlugin::onLayerSelectionChanged);

    // Do an initial update of the window title
    onLayerSelectionChanged();

    // Routine to show the context menu
    connect(&_imageViewerWidget, &ImageViewerWidget::customContextMenuRequested, this, [this](const QPoint& point) {

        // Only show a context menu when there is data loaded
        if (_model.rowCount() <= 0)
            return;

        // Show the context menu
        _settingsAction.getContextMenu()->exec(getWidget().mapToGlobal(point));
    });

    _dropWidget.setShowDropIndicator(false);
    _dropWidget.setShowDropIndicator(true);
}

void ImageViewerPlugin::loadData(const Datasets& datasets)
{
    // Only load data if we at least have one set
    if (datasets.isEmpty())
        return;

    // Add datasets one-by-one
    for (const auto& dataset : datasets)
        addDataset(dataset);
}

void ImageViewerPlugin::arrangeLayers(LayersLayout layersLayout)
{
    auto layers = _model.getLayers();

    const auto numberOfLayers   = layers.size();
    
    auto margin = 0.0f;

    for (auto layer : layers) {
        const auto candidateMargin = 0.15f * std::max(layer->getWorldBoundingRectangle().width(), layer->getWorldBoundingRectangle().height());

        if (candidateMargin > margin)
            margin = candidateMargin;
    }

    switch (layersLayout)
    {
        case LayersLayout::Stacked:
            break;

        case LayersLayout::Vertical:
        {
            auto offsetY = 0u;

            for (int layerIndex = 0; layerIndex < numberOfLayers; ++layerIndex) {
                auto currentLayer   = layers[layerIndex];
                auto nextLayer      = layers[(layerIndex + 1) < numberOfLayers ? layerIndex + 1 : layerIndex];

                layers[layerIndex]->getGeneralAction().getPositionAction().getYAction().setValue(offsetY);
                
                offsetY += currentLayer->getWorldBoundingRectangle().height() / 2;
                offsetY += margin;
                offsetY += nextLayer->getWorldBoundingRectangle().height() / 2;
            }

            break;
        }

        case LayersLayout::Horizontal:
        {
            auto offsetX = 0u;

            for (int layerIndex = 0; layerIndex < numberOfLayers; ++layerIndex) {
                auto currentLayer   = layers[layerIndex];
                auto nextLayer      = layers[(layerIndex + 1) < numberOfLayers ? layerIndex + 1 : layerIndex];

                layers[layerIndex]->getGeneralAction().getPositionAction().getXAction().setValue(offsetX);

                offsetX += currentLayer->getWorldBoundingRectangle().width() / 2;
                offsetX += margin;
                offsetX += nextLayer->getWorldBoundingRectangle().width() / 2;
            }

            break;
        }

        case LayersLayout::Grid:
        {
            const auto numberOfColumns  = static_cast<std::uint32_t>(ceilf(sqrtf(static_cast<float>(numberOfLayers))));
            const auto numberOfRows     = static_cast<std::uint32_t>(ceilf(static_cast<float>(numberOfLayers) / static_cast<float>(numberOfColumns)));

            QVector<float> columnWidths, rowHeights;

            columnWidths.resize(numberOfColumns, 0.0f);
            rowHeights.resize(numberOfRows, 0.0f);

            for (int columnIndex = 0; columnIndex < numberOfColumns; ++columnIndex) {
                for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                    auto layer = layers[rowIndex * numberOfColumns + columnIndex];
                    
                    const auto layerWidth = layer->getWorldBoundingRectangle().width();

                    if (layerWidth > columnWidths[columnIndex])
                        columnWidths[columnIndex] = layerWidth;
                }
            }

            for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                for (int columnIndex = 0; columnIndex < numberOfColumns; ++columnIndex) {
                    auto layer = layers[rowIndex * numberOfColumns + columnIndex];

                    const auto layerHeight = layer->getWorldBoundingRectangle().height();

                    if (layerHeight > rowHeights[rowIndex])
                        rowHeights[rowIndex] = layerHeight;
                }
            }

            QPoint offset;

            for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                for (int columnIndex = 0; columnIndex < numberOfColumns; ++columnIndex) {
                    auto layer = layers[rowIndex * numberOfColumns + columnIndex];

                    layer->getGeneralAction().getPositionAction().getXAction().setValue(offset.x());
                    layer->getGeneralAction().getPositionAction().getYAction().setValue(offset.y());

                    offset.setX(offset.x() + columnWidths[columnIndex] + margin);
                }

                offset.setX(0);
                offset.setY(offset.y() - rowHeights[rowIndex] - margin);
            }

            break;
        }
    }

    getImageViewerWidget().getRenderer().setZoomRectangle(getImageViewerWidget().getWorldBoundingRectangle());
    getImageViewerWidget().update();
}

void ImageViewerPlugin::addDataset(const Dataset<Images>& dataset)
{
    // Create new layer for the converted dataset
    auto layer = new Layer(*this, dataset);

    // Squeeze the layer in to the layers world bounding rectangle
    layer->scaleToFit(_imageViewerWidget.getWorldBoundingRectangle(false));

    // Add new layer to the model
    _model.addLayer(layer);

    // Update bounds
    _imageViewerWidget.updateWorldBoundingRectangle();

    // Zoom when this is the first layer added
    if (_model.rowCount() == 1)
        layer->zoomToExtents();
}

void ImageViewerPlugin::onLayerSelectionChanged()
{
    return;
    // Get selected row and establish whether there is a valid selection
    const auto selectedRows = _selectionModel.selectedRows();
    const auto hasSelection = !selectedRows.isEmpty();

    // Name of the currently selected layer
    QString currentLayerName = "";

    // Update current layer name when there is a valid selection
    if (hasSelection) {

        // Get pointer to layer that was selected
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // A layer is selected so change the current layer name
        currentLayerName = layer->getGeneralAction().getNameAction().getString();

        // Zoom to the extents of the layer if smart zoom is enabled
        if (_mainToolbarAction->getGlobalViewSettingsAction().getSmartZoomAction().isChecked() && layer->getGeneralAction().getVisibleAction().isChecked())
            layer->zoomToExtents();
    }

    // Update the window title
    getWidget().setWindowTitle(QString("%1%2").arg(getGuiName(), currentLayerName.isEmpty() ? "" : QString(": %1").arg(currentLayerName)));
}

void ImageViewerPlugin::immigrateDataset(const Dataset<DatasetImpl>& dataset)
{
    try {
        // don't call QDialog.exec() or use a static method like QDialog::getOpenFileName
        // since they trigger some assertion failures due to threading issues when opening the dialog here

        // Create conversion dialog
        ConvertToImagesDatasetDialog* dialog = new ConvertToImagesDatasetDialog(*this, const_cast<Dataset<DatasetImpl>&>(dataset));

        connect(dialog, &ConvertToImagesDatasetDialog::accepted, this, [this, dialog]() -> void {
            // Create new layer for the converted dataset
            auto layer = new Layer(*this, dialog->getTargetImagesDataset());

            // Squeeze the layer in to the layers world bounding rectangle
            layer->scaleToFit(_imageViewerWidget.getWorldBoundingRectangle(false));

            // Add new layer to the model
            _model.addLayer(layer);

            // Update world bounds of all layers
            _imageViewerWidget.updateWorldBoundingRectangle();

            // Zoom to the extents of the layer if smart zoom is enabled
            if (_mainToolbarAction->getGlobalViewSettingsAction().getSmartZoomAction().isChecked() || _model.rowCount() == 1)
                layer->zoomToExtents();

            dialog->deleteLater();
            });

        // Show the dialog and add the layer if accepted
        dialog->open();

    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(dataset->getGuiName()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(dataset->getGuiName()));
    }
}

QIcon ImageViewerPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images", color);
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin(this);
}

PluginTriggerActions ImageViewerPluginFactory::getPluginTriggerActions(const hdps::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;

    const auto getInstance = [this]() -> ImageViewerPlugin* {
        return dynamic_cast<ImageViewerPlugin*>(Application::core()->requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (PluginFactory::areAllDatasetsOfTheSameType(datasets, ImageType)) {
        auto& fontAwesome = Application::getIconFont("FontAwesome");

        if (numberOfDatasets == 1) {
            if (datasets.first()->getDataType() == ImageType) {
                auto pluginTriggerAction = new PluginTriggerAction(const_cast<ImageViewerPluginFactory*>(this), this, "Image Viewer", "Load dataset in image viewer", fontAwesome.getIcon("images"), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                    getInstance()->loadData(datasets);
                });

                pluginTriggerActions << pluginTriggerAction;
            }
        }
        
        if (numberOfDatasets >= 2) {
            auto viewTogetherAction = new PluginTriggerAction(const_cast<ImageViewerPluginFactory*>(this), this, "Images/Stacked", "View datasets in the image viewer arranged on top of each other", fontAwesome.getIcon("layer-group"), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                getInstance()->loadData(datasets);
            });

            auto arrangeVerticallyAction = new PluginTriggerAction(const_cast<ImageViewerPluginFactory*>(this), this, "Images/Vertically", "View datasets in the image viewer arranged vertically", fontAwesome.getIcon("long-arrow-alt-down"), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                auto plugin = getInstance();
                
                plugin->loadData(datasets);
                plugin->arrangeLayers(ImageViewerPlugin::LayersLayout::Vertical);
            });

            auto arrangeHorizontallyAction = new PluginTriggerAction(const_cast<ImageViewerPluginFactory*>(this), this, "Images/Horizontally", "View datasets in the image viewer arranged horizontally", fontAwesome.getIcon("long-arrow-alt-right"), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                auto plugin = getInstance();

                plugin->loadData(datasets);
                plugin->arrangeLayers(ImageViewerPlugin::LayersLayout::Horizontal);
            });

            auto arrangeGridAction = new PluginTriggerAction(const_cast<ImageViewerPluginFactory*>(this), this, "Images/Grid", "View datasets in the image viewer arranged in a grid", fontAwesome.getIcon("th"), [this, getInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
                auto plugin = getInstance();

                plugin->loadData(datasets);
                plugin->arrangeLayers(ImageViewerPlugin::LayersLayout::Grid);
            });

            pluginTriggerActions << viewTogetherAction << arrangeVerticallyAction << arrangeHorizontallyAction << arrangeGridAction;
        }
    }

	return pluginTriggerActions;
}
