#include "ImageViewerPlugin.h"
#include "SelectionAction.h"
#include "SettingsAction.h"
#include "NavigationAction.h"
#include "Layer.h"
#include "PointsToImagesDialog.h"

#include "ImageData/Images.h"
#include "ImageData/ImageData.h"
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
    _dropWidget(nullptr),
    _mainWidget(nullptr),
    _splitter(new QSplitter()),
    _imageViewerWidget(nullptr),
    _settingsAction(nullptr),
    _navigationAction(nullptr)
{
    setFocusPolicy(Qt::ClickFocus);
}

void ImageViewerPlugin::init()
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    auto viewerLayout   = new QVBoxLayout();

    _mainWidget         = new QWidget();
    _imageViewerWidget  = new ImageViewerWidget(this, _model);
    _settingsAction     = new SettingsAction(*this);
    _navigationAction   = new NavigationAction(*this);

    _imageViewerWidget->setAcceptDrops(true);

    _dropWidget = new DropWidget(_imageViewerWidget);

    auto mainWidgetLayout = new QVBoxLayout();

    // Configure main layout
    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing(0);

    // And add the toolbar, image viewer widget
    mainWidgetLayout->addWidget(_imageViewerWidget, 1);
    mainWidgetLayout->addWidget(_navigationAction->createWidget(this));

    // Apply layout to main widget
    _mainWidget->setLayout(mainWidgetLayout);

    // Add viewer widget and settings panel to the splitter
    _splitter->addWidget(_mainWidget);
    _splitter->addWidget(_settingsAction->createWidget(this));

    // Configure splitter
    _splitter->setStretchFactor(0, 1);
    _splitter->setStretchFactor(1, 0);
    _splitter->setCollapsible(1, true);

    layout->addWidget(_splitter);

    setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText     = mimeData->text();
        const auto tokens       = mimeText.split("\n");
        const auto datasetName  = tokens[0];
        const auto dataType     = hdps::DataType(tokens[1]);
        const auto dataTypes    = hdps::DataTypes({ ImageType, PointType });

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == ImageType) {
            dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                try
                {
                    _model.addLayer(SharedLayer::create(*this, datasetName));

                    // Update bounds
                    _imageViewerWidget->updateWorldBoundingRectangle();
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
            dropRegions << new DropWidget::DropRegion(this, "Points", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                try
                {
                    // Create conversion dialog
                    PointsToImagesDialog pointsToImagesDialog(datasetName, this);

                    // Show the dialog and catch the result
                    const auto result = pointsToImagesDialog.exec();

                    if (result == 1) {
                        DatasetRef<Images> images(_core->addData("Images", "images", datasetName));

                        if (!images.isValid())
                            throw std::runtime_error("Unable to create images dataset");

                        images->setType(ImageData::Type::Stack);
                        images->setNumberOfImages(pointsToImagesDialog.getNumberOfImagesAction().getValue());
                        images->setImageGeometry(pointsToImagesDialog.getImageSize());
                        images->setNumberOfComponentsPerPixel(1);

                        _core->notifyDataAdded(images->getName());
                    }
                    
                    // Update bounds
                    _imageViewerWidget->updateWorldBoundingRectangle();
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

        return dropRegions;
    });

    connect(_imageViewerWidget, &ImageViewerWidget::pixelSelectionStarted, this, [this]() {

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

    connect(_imageViewerWidget, &ImageViewerWidget::mousePositionsChanged, this, [this](const QVector<QPoint>& mousePositions) {

        // No point in computing selection when there are no mouse positions
        if (mousePositions.count() <= 1)
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

    // 
    connect(_imageViewerWidget, &ImageViewerWidget::pixelSelectionEnded, this, [this]() {

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
        _dropWidget->setShowDropIndicator(_model.rowCount() == 0);

        // Establish the number of visible layers
        const auto hasVisibleLayers = _model.rowCount() == 0 ? false : !_model.match(_model.index(0, LayersModel::Visible), Qt::EditRole, true, -1).isEmpty();

        // Enabled/disable navigation tool bar
        _navigationAction->setEnabled(hasVisibleLayers);
    };

    // Enable/disable the navigation action when rows are inserted/removed
    connect(&_model, &LayersModel::rowsInserted, this, layersInsertedRemovedChanged);
    connect(&_model, &LayersModel::rowsRemoved, this, layersInsertedRemovedChanged);
    connect(&_model, &LayersModel::dataChanged, this, layersInsertedRemovedChanged);

    // Initially enable/disable the navigation action
    layersInsertedRemovedChanged();

    // Update the window title
    const auto updateWindowTitle = [this]() {
        
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
        }

        // Update the window title
        setWindowTitle(QString("%1%2").arg(getGuiName(), currentLayerName.isEmpty() ? "" : QString(": %1").arg(currentLayerName)));
    };

    // Change the window title when the layer selection or layer name changes
    connect(&_selectionModel, &QItemSelectionModel::selectionChanged, this, updateWindowTitle);
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
