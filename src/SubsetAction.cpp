#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include "util/Exception.h"

#include "PointData.h"

using namespace hdps;

SubsetAction::SubsetAction(ImageViewerPlugin& imageViewerPlugin) :
    TriggerAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _nameAction(this, "Name"),
    _fromRegionAction(this, "From region", true, true),
    _createAction(this, "Create")
{
    setText("Create subset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("crop"));

    _nameAction.setToolTip("Name of the subset");
    _fromRegionAction.setToolTip("Create an image from the selected region");
    _createAction.setToolTip("Create the subset");

    // Actions may not be reset
    _fromRegionAction.setMayReset(false);
    _nameAction.setMayReset(false);

    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& newSelection, const QItemSelection& oldSelection) {
        
        // Process deselected layers
        if (!oldSelection.indexes().isEmpty()) {

            // Get pointer to layer that was deselected
            auto layer = static_cast<Layer*>(oldSelection.indexes().first().internalPointer());

            // Disconnect previously selected layer
            disconnect(layer, &Layer::selectionChanged, this, nullptr);
        }

        // Process selected layers
        if (!newSelection.indexes().isEmpty()) {

            // Get pointer to layer that was selected
            auto layer = static_cast<Layer*>(newSelection.indexes().first().internalPointer());

            // Enable/disable
            const auto updateEnabled = [this, layer]() -> void {

                // Establish conditions
                const auto isPointType  = layer->getSourceDataset()->getDataType() == PointType;
                const auto hasSelection = !layer->getSelectedIndices().empty();

                setEnabled(isPointType && hasSelection);
            };

            // Enable/disable when the layer selection changes
            connect(layer, &Layer::selectionChanged, this, updateEnabled);

            // Do an initial update when the layer is selected
            updateEnabled();
        }
    });

    // Update the state of the create button
    const auto updateCreateButton = [this]() {
        _createAction.setEnabled(!_nameAction.getString().isEmpty());
    };

    // Update the state of the create button when the name changes
    connect(&_nameAction, &StringAction::stringChanged, this, updateCreateButton);

    // Do an initial update
    updateCreateButton();

    // Create the subset when the create action is triggered
    connect(&_createAction, &TriggerAction::triggered, this, [this]() -> void {

        try {

            // Get selected row from selection model
            const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

            // Only accept one selected layer at a time
            if (selectedRows.isEmpty())
                throw std::runtime_error("No layer selected");

            // Get pointer to selected layer
            auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

            // Pointer to points dataset
            auto points = layer->getSourceDataset<Points>();

            // Get reference to images dataset
            auto images = layer->getImages();

            if (_fromRegionAction.isChecked()) {

                // Get the image size
                const auto imageSize = layer->getImageSize();

                // Cache the selection indices
                auto cachedSelectionIndices = layer->getSelectedIndices();

                // Get the selection rectangle in image coordinates
                const auto selectionBoundaries = layer->getSelectionAction().getImageSelectionRectangle();

                // Compute the number of pixels in the region
                const auto numberOfPixelsInRegion = selectionBoundaries.width() * selectionBoundaries.height();

                // Get reference to selection indices
                auto& selectionIndices = points->getSelection<Points>()->indices;

                // Allocate space for indices
                selectionIndices.clear();
                selectionIndices.reserve(numberOfPixelsInRegion);

                // Populate new selection indices
                for (std::int32_t roiPixelY = selectionBoundaries.top(); roiPixelY <= selectionBoundaries.bottom(); roiPixelY++)
                    for (std::int32_t roiPixelX = selectionBoundaries.left(); roiPixelX <= selectionBoundaries.right(); roiPixelX++)
                        selectionIndices.push_back(roiPixelY * imageSize.width() + roiPixelX);

                // Except when selection set is empty
                if (selectionIndices.empty())
                    throw std::runtime_error("Selection is empty");

                // Create the points subset
                auto subset = layer->getSourceDataset<Points>()->createSubset(_nameAction.getString(), points);

                // Notify that the points set was added
                Application::core()->notifyDataAdded(subset);

                // Reset selected indices
                selectionIndices = cachedSelectionIndices;

                // Create a new image dataset which is a subset of the original image
                auto imagesSubset = Application::core()->addDataset<Images>("Images", _nameAction.getString(), points);

                imagesSubset->setType(images->getType());
                imagesSubset->setNumberOfImages(images->getNumberOfImages());
                imagesSubset->setImageGeometry(images->getSourceRectangle().size(), selectionBoundaries.size(), selectionBoundaries.topLeft());
                imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());
                //imagesSubset->setImageFilePaths(images->getImag);

                // Notify others that the images dataset was added
                Application::core()->notifyDataAdded(*imagesSubset);

                // Reset the name
                _nameAction.reset();
            }
            else {
                
                // Create the points subset
                auto subset = points->createSubset(_nameAction.getString(), points);

                // Notify that the points set was added
                Application::core()->notifyDataAdded(subset);
            }
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to set create subset", e);
        }
        catch (...) {
            exceptionMessageBox("Unable to set create subset");
        }
    });

    // Highlight the selection region in the viewer when needed
    connect(&_fromRegionAction, &ToggleAction::toggled, this, &SubsetAction::updateHighlightRegion);
}

void SubsetAction::updateHighlightRegion()
{
    // Get selected row from selection model
    const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

    // Only accept one selected layer at a time
    if (selectedRows.isEmpty())
        return;

    // Get pointer to selected layer
    auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

    // Show/hide the region
    layer->getSelectionAction().getShowRegionAction().setChecked(getFromRegionAction().isChecked());
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, subsetAction),
    _subsetAction(subsetAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(subsetAction->getNameAction().createLabelWidget(this), 0, 0);
    layout->addWidget(subsetAction->getNameAction().createWidget(this), 0, 1);
    layout->addWidget(subsetAction->getFromRegionAction().createWidget(this), 1, 1);
    layout->addWidget(subsetAction->getCreateAction().createWidget(this), 2, 1);

    // Close the widget when the create action is triggered
    connect(&subsetAction->getCreateAction(), &TriggerAction::triggered, this, [this]() {
        parentWidget()->close();
    });

    // Highlight the selection region when needed
    subsetAction->updateHighlightRegion();

    if (widgetFlags & PopupLayout)
    {
        setPopupLayout(layout);
    }
    else {
        layout->setMargin(0);
        setLayout(layout);
    }
}

SubsetAction::Widget::~Widget()
{
    // Get selected row from selection model
    const auto selectedRows = _subsetAction->getImageViewerPlugin().getSelectionModel().selectedRows();

    // Only accept one selected layer at a time
    if (selectedRows.isEmpty())
        return;

    // Get pointer to selected layer
    auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

    // Hide the selection region in the viewer
    layer->getSelectionAction().getShowRegionAction().setChecked(false);
}
