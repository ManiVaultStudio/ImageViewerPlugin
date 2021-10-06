#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include "util/Exception.h"

using namespace hdps;

SubsetAction::SubsetAction(Layer& layer) :
    GroupAction(&layer, true),
    hdps::EventListener(),
    _layer(layer),
    _fromRegionAction(this, "From region", true, true),
    _nameAction(this, "Name"),
    _createAction(this, "Create")
{
    setText("Subset");
    setEventCore(Application::core());

    _fromRegionAction.setToolTip("Create subset from rectangular region");
    _nameAction.setToolTip("Name of the layer");
    _createAction.setToolTip("Create the subset");
    
    // Update actions states
    const auto updateActionStates = [this]() {
        
        // Establish whether there is a valid selection 
        const auto hasSelection = !_layer.getSelectedPixels().empty();

        // Enable/disable actions
        _nameAction.setEnabled(hasSelection);
        _createAction.setEnabled(hasSelection && !_nameAction.getString().isEmpty());
    };

    // Update action state(s) when the subset name changes
    connect(&_nameAction, &StringAction::stringChanged, this, updateActionStates);

    // Update action state(s) when the selection changes
    registerDataEventByType(PointType, [this, updateActionStates](hdps::DataEvent* dataEvent) {
        if (dataEvent->getType() == hdps::EventType::SelectionChanged) {
            auto selectionChangedEvent = static_cast<hdps::SelectionChangedEvent*>(dataEvent);

            if (DatasetRef<Points>(selectionChangedEvent->dataSetName).getSourceData().getName() == _layer.getPoints()->getName())
                updateActionStates();
        }
    });

    // Perform an initial update of the actions
    updateActionStates();

    // Create the subset
    connect(&_createAction, &TriggerAction::triggered, this, [this]() {
        try {

            auto& points = _layer.getPoints();
            auto& images = _layer.getImages();

            if (_fromRegionAction.isChecked()) {

                // Get the image size
                const auto imageSize = _layer.getImageSize();

                // Cache the selection indices
                auto cachedSelectionIndices = _layer.getSelectedPixels();

                // Get the selection boundaries
                const auto selectionBoundaries = _layer.getSelectionAction().getSelectionBoundaries();

                // Compute the number of pixels in the region
                const auto numberOfPixelsInRegion = selectionBoundaries.width() * selectionBoundaries.height();

                // Get reference to selection indices
                auto& modifySelectionIndices = _layer.getSelectedPixels();

                // Allocate space for indices
                modifySelectionIndices.clear();
                modifySelectionIndices.reserve(numberOfPixelsInRegion);

                // Populate new selection indices
                for (std::int32_t roiPixelX = selectionBoundaries.left(); roiPixelX <= selectionBoundaries.right(); roiPixelX++)
                    for (std::int32_t roiPixelY = selectionBoundaries.top(); roiPixelY <= selectionBoundaries.bottom(); roiPixelY++)
                        modifySelectionIndices.push_back(roiPixelY * imageSize.width() + roiPixelX);

                // Except when selection set is empty
                if (modifySelectionIndices.empty())
                    throw std::runtime_error("Selection is empty");

                // Create the points subset
                DatasetRef<Points> pointsSubset(points.getSourceData().createSubset(_nameAction.getString(), points->getName()));

                // Notify that the points set was added
                Application::core()->notifyDataAdded(pointsSubset.getDatasetName());

                // And re-instate the cached selection
                modifySelectionIndices = cachedSelectionIndices;

                // Create a new image dataset which is a subset of the original image
                DatasetRef<Images> imagesSubset(Application::core()->addData("Images", _nameAction.getString(), pointsSubset.getDatasetName()));

                imagesSubset->setType(images->getType());
                imagesSubset->setNumberOfImages(images->getNumberOfImages());
                imagesSubset->setImageGeometry(imageSize, selectionBoundaries.size(), selectionBoundaries.topLeft());
                imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());
                //imagesSubset->setImageFilePaths(images->getImag);
                imagesSubset->setDimensionNames(images->getDimensionNames());

                Application::core()->notifyDataAdded(imagesSubset.getDatasetName());
            }
            else {
                // Create the points subset
                DatasetRef<Points> pointsSubset(points.getSourceData().createSubset(_nameAction.getString(), points->getName()));

                // Notify that the points set was added
                Application::core()->notifyDataAdded(pointsSubset.getDatasetName());
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
}
