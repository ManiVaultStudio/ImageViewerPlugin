#include "ImageViewerPlugin.h"
#include "SelectionAction.h"
#include "Layer.h"
#include "ConvertToImagesDatasetDialog.h"

#include "ImageData/Images.h"
#include "ImageData/ImageData.h"
#include "ClusterData.h"
#include "widgets/DropWidget.h"
#include "util/DatasetRef.h"
#include "util/Exception.h"

#include <QDebug>
#include <QSplitter>
#include <QMimeData>

using namespace hdps::gui;
using namespace hdps::util;

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin(hdps::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _model(this),
    _selectionModel(&_model),
    _splitter(Qt::Horizontal, this),
    _imageViewerWidget(this, _model),
    _dropWidget(&_imageViewerWidget),
    _mainToolbarAction(*this),
    _zoomToolbarAction(*this),
    _settingsAction(*this)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::ClickFocus);
}

void ImageViewerPlugin::init()
{
    // Create main layout for view and editing
    auto mainLayout = new QHBoxLayout();

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    // Create main left widget
    auto mainWidget = new QWidget();

    // Create main widget layout
    auto mainWidgetLayout = new QVBoxLayout();

    // Configure main layout
    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing(0);

    // And add the toolbar, image viewer widget
    mainWidgetLayout->addWidget(_mainToolbarAction.createWidget(this));
    mainWidgetLayout->addWidget(&_imageViewerWidget, 1);
    mainWidgetLayout->addWidget(_zoomToolbarAction.createWidget(this));

    // Apply layout to main widget
    mainWidget->setLayout(mainWidgetLayout);

    // Add viewer widget and settings panel to the splitter
    _splitter.addWidget(mainWidget);
    _splitter.addWidget(_settingsAction.createWidget(this));

    // Configure splitter
    _splitter.setStretchFactor(0, 1);
    _splitter.setStretchFactor(1, 0);
    _splitter.setCollapsible(1, true);

    // Add splitter to the main layout
    mainLayout->addWidget(&_splitter);

    // Provide a hint on how to load data
    _dropWidget.setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    // Establish droppable regions
    _dropWidget.initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText     = mimeData->text();
        const auto tokens       = mimeText.split("\n");
        const auto datasetName  = tokens[0];
        const auto dataType     = hdps::DataType(tokens[1]);
        const auto dataTypes    = hdps::DataTypes({ ImageType, PointType, ClusterType });

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == ImageType) {
            dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                try
                {
                    // Create new layer for the converted dataset
                    auto layer = new Layer(*this, datasetName);

                    // Add new layer to the model
                    _model.addLayer(layer);

                    // Update bounds
                    _imageViewerWidget.updateWorldBoundingRectangle();

                    // Squeeze the layer in to the layers world bounding rectangle
                    //layer->fitInRectangle(_imageViewerWidget.getWorldBoundingRectangle());
                }
                catch (std::exception& e)
                {
                    exceptionMessageBox(QString("Unable to load '%1'").arg(datasetName), e);
                }
                catch (...) {
                    exceptionMessageBox(QString("Unable to load '%1'").arg(datasetName));
                }
            });
        }

        if (dataType == PointType) {
            dropRegions << new DropWidget::DropRegion(this, "Points", QString("Convert %1 to image layer").arg(datasetName), true, [this, datasetName]() {

                // Convert the points dataset to an images dataset and add as a layer
                immigrateDataset(datasetName);
            });
        }

        if (dataType == ClusterType) {
            dropRegions << new DropWidget::DropRegion(this, "Clusters", QString("Convert %1 to image layer").arg(datasetName), true, [this, datasetName]() {

                // Convert the points dataset to an images dataset and add as a layer
                immigrateDataset(datasetName);
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
        const auto isSampleSelection = layer->getSelectionAction().getTypeAction().getCurrentIndex() == static_cast<std::int32_t>(PixelSelectionType::Sample);

        // Publish the selection
        if (selectionAction.getNotifyDuringSelectionAction().isChecked() || isSampleSelection)
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
        _settingsAction.getContextMenu()->exec(mapToGlobal(point));
    });

    _dropWidget.setShowDropIndicator(true);
}

void ImageViewerPlugin::onLayerSelectionChanged()
{
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
        if (_mainToolbarAction.getGlobalViewSettingsAction().getSmartZoomAction().isChecked())
            layer->zoomToExtents();
    }

    // Update the window title
    setWindowTitle(QString("%1%2").arg(getGuiName(), currentLayerName.isEmpty() ? "" : QString(": %1").arg(currentLayerName)));
}

void ImageViewerPlugin::immigrateDataset(const QString& datasetName)
{
    try {

        // Create conversion dialog
        ConvertToImagesDatasetDialog pointsToImagesDialog(*this, datasetName);

        // Show the dialog and add the layer if accepted
        if (pointsToImagesDialog.exec() == 1) {

            // Create new layer for the converted dataset
            auto layer = new Layer(*this, pointsToImagesDialog.getTargetImagesDatasetName());

            // Add new layer to the model
            _model.addLayer(layer);

            // Update world bounds of all layers
            _imageViewerWidget.updateWorldBoundingRectangle();

            // Squeeze the layer in to the layers world bounding rectangle
            //layer->fitInRectangle(_imageViewerWidget.getWorldBoundingRectangle());

            // Zoom to the extents of the layer if smart zoom is enabled
            if (_mainToolbarAction.getGlobalViewSettingsAction().getSmartZoomAction().isChecked())
                layer->zoomToExtents();
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(datasetName), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(datasetName));
    }
}

QIcon ImageViewerPluginFactory::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin(this);
}

hdps::DataTypes ImageViewerPluginFactory::supportedDataTypes() const
{
    hdps::DataTypes supportedTypes;

    supportedTypes.append(ImageType);

    return supportedTypes;
}
