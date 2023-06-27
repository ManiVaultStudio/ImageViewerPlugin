#include "ImageViewerPlugin.h"
#include "SelectionAction.h"
#include "Layer.h"
#include "ConvertToImagesDatasetDialog.h"

#include <ImageData/Images.h>
#include <ImageData/ImageData.h>
#include <ClusterData/ClusterData.h>
#include <util/Exception.h>
#include <actions/PluginTriggerAction.h>
#include <DatasetsMimeData.h>

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
    _layersModel(this),
    _selectionModel(&_layersModel),
    _splitter(Qt::Horizontal, &getWidget()),
    _imageViewerWidget(*this),
    _dropWidget(&_imageViewerWidget),
    _selectionToolbarAction(*this),
    _interactionToolbarAction(this, "Zoom Toolbar"),
    _settingsAction(this, "Settings")
{
    setObjectName("Images");

    getWidget().setContextMenuPolicy(Qt::CustomContextMenu);
    getWidget().setFocusPolicy(Qt::ClickFocus);

    _interactionToolbarAction.initialize(this);
}

void ImageViewerPlugin::init()
{
    auto mainLayout = new QHBoxLayout();

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    getWidget().setLayout(mainLayout);

    auto mainWidget = new QWidget();

    auto mainWidgetLayout = new QVBoxLayout();

    mainWidgetLayout->setContentsMargins(0, 0, 0, 0);
    mainWidgetLayout->setSpacing(0);

    //mainWidgetLayout->addWidget(_selectionToolbarAction.createWidget(&getWidget()));
    mainWidgetLayout->addWidget(&_imageViewerWidget, 1);
    //mainWidgetLayout->addWidget(_interactionToolbarAction.createWidget(&getWidget()));

    mainWidget->setLayout(mainWidgetLayout);

    _splitter.addWidget(mainWidget);
    //_splitter.addWidget(_settingsAction.createWidget(&getWidget()));

    _splitter.setStretchFactor(0, 1);
    _splitter.setStretchFactor(1, 0);
    _splitter.setCollapsible(1, true);

    mainLayout->addWidget(&_splitter);

    _dropWidget.setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(&getWidget(), "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget.initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto datasetsMimeData = dynamic_cast<const DatasetsMimeData*>(mimeData);

        if (datasetsMimeData == nullptr)
            return dropRegions;

        if (datasetsMimeData->getDatasets().count() > 1)
            return dropRegions;

        const auto dataset          = datasetsMimeData->getDatasets().first();
        const auto datasetGuiName   = dataset->text();
        const auto datasetId        = dataset->getId();
        const auto dataType         = dataset->getDataType();
        const auto dataTypes        = hdps::DataTypes({ ImageType, PointType, ClusterType });

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
                immigrateDataset(dataset);
            });
        }

        if (dataType == ClusterType) {
            dropRegions << new DropWidget::DropRegion(this, "Clusters", QString("Convert %1 to image layer").arg(datasetGuiName), "th-large", true, [this, dataset]() {
                immigrateDataset(dataset);
            });
        }

        return dropRegions;
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::pixelSelectionStarted, this, [this]() {
        const auto selectedRows = _selectionModel.selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = _layersModel.getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return;

        layer->startSelection();
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::mousePositionsChanged, this, [this](const QVector<QPoint>& mousePositions) {
        if (mousePositions.count() == 0)
            return;

        const auto selectedRows = _selectionModel.selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = _layersModel.getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return;

        layer->computeSelection(mousePositions);

        auto& selectionAction = layer->getSelectionAction();

        const auto isSampleSelection = layer->getSelectionAction().getPixelSelectionAction().getTypeAction().getCurrentIndex() == static_cast<std::int32_t>(PixelSelectionType::Sample);

        if (selectionAction.getPixelSelectionAction().getNotifyDuringSelectionAction().isChecked() || isSampleSelection)
            layer->publishSelection();

        if (isSampleSelection)
            layer->resetSelectionBuffer();
    });

    connect(&_imageViewerWidget, &ImageViewerWidget::pixelSelectionEnded, this, [this]() {
        const auto selectedRows = _selectionModel.selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = _layersModel.getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return;

        layer->publishSelection();
        layer->resetSelectionBuffer();
    });

    const auto layersInsertedRemovedChanged = [this]() {
        _dropWidget.setShowDropIndicator(_layersModel.rowCount() == 0);

        const auto hasVisibleLayers = _layersModel.rowCount() == 0 ? false : !_layersModel.match(_layersModel.index(0, static_cast<int>(LayersModel::Column::Visible)), Qt::EditRole, true, -1).isEmpty();

        _selectionToolbarAction.setEnabled(hasVisibleLayers);
        _interactionToolbarAction.setEnabled(hasVisibleLayers);
    };

    connect(&_layersModel, &LayersModel::rowsInserted, this, layersInsertedRemovedChanged);
    connect(&_layersModel, &LayersModel::rowsRemoved, this, layersInsertedRemovedChanged);
    connect(&_layersModel, &LayersModel::dataChanged, this, layersInsertedRemovedChanged);

    layersInsertedRemovedChanged();

    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, &ImageViewerPlugin::onLayerSelectionChanged);

    onLayerSelectionChanged();

    connect(&_imageViewerWidget, &ImageViewerWidget::customContextMenuRequested, this, [this](const QPoint& point) {
        if (_layersModel.rowCount() <= 0)
            return;

        _settingsAction.getContextMenu()->exec(getWidget().mapToGlobal(point));
    });

    _dropWidget.setShowDropIndicator(false);
    _dropWidget.setShowDropIndicator(true);

    addSettingsAction(&_settingsAction, nullptr, DockAreaFlag::Left, true, AutoHideLocation::Right, QSize(300, 300));
}

void ImageViewerPlugin::loadData(const Datasets& datasets)
{
    if (datasets.isEmpty())
        return;

    for (const auto& dataset : datasets)
        addDataset(dataset);
}

void ImageViewerPlugin::arrangeLayers(LayersLayout layersLayout)
{
    auto layers = _layersModel.getLayers();

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
    auto layer = new Layer(&_settingsAction.getEditLayersAction(), dataset->text());

    layer->initialize(this, dataset);
    layer->scaleToFit(_imageViewerWidget.getWorldBoundingRectangle(false));

    _layersModel.addLayer(layer);
}

void ImageViewerPlugin::onLayerSelectionChanged()
{
    /*
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
        if (_interactionToolbarAction.getViewSettingsAction().getSmartZoomAction().isChecked() && layer->getGeneralAction().getVisibleAction().isChecked())
            layer->zoomToExtents();
    }

    // Update the window title
    getWidget().setWindowTitle(QString("%1%2").arg(getGuiName(), currentLayerName.isEmpty() ? "" : QString(": %1").arg(currentLayerName)));
    */
}

void ImageViewerPlugin::immigrateDataset(const Dataset<DatasetImpl>& dataset)
{
    try {
        ConvertToImagesDatasetDialog* dialog = new ConvertToImagesDatasetDialog(*this, const_cast<Dataset<DatasetImpl>&>(dataset));

        connect(dialog, &ConvertToImagesDatasetDialog::accepted, this, [this, dialog]() -> void {
            auto layer = new Layer(&_settingsAction.getEditLayersAction(), dialog->getTargetImagesDataset()->text());

            layer->initialize(this, dialog->getTargetImagesDataset());
            layer->scaleToFit(_imageViewerWidget.getWorldBoundingRectangle(false));

            _layersModel.addLayer(layer);

            _imageViewerWidget.updateWorldBoundingRectangle();

            if (_interactionToolbarAction.getViewSettingsAction().getSmartZoomAction().isChecked() || _layersModel.rowCount() == 1)
                layer->zoomToExtents();

            dialog->deleteLater();
        });

        dialog->open();

    }
    catch (std::exception& e)
    {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(dataset->text()), e);
    }
    catch (...) {
        exceptionMessageBox(QString("Unable to immigrate dataset: %1").arg(dataset->text()));
    }
}

void ImageViewerPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    _layersModel.fromParentVariantMap(variantMap);
}

QVariantMap ImageViewerPlugin::toVariantMap() const
{
    auto variantMap = ViewPlugin::toVariantMap();

    _layersModel.insertIntoVariantMap(variantMap);

    return variantMap;
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
        return dynamic_cast<ImageViewerPlugin*>(Application::core()->getPluginManager().requestViewPlugin(getKind()));
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