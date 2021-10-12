#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"
#include "CreateSubsetDialog.h"

#include "util/Exception.h"

using namespace hdps;

SubsetAction::SubsetAction(ImageViewerPlugin& imageViewerPlugin) :
    TriggerAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin)
{
    setText("Subset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("crop"));

    connect(&_imageViewerPlugin.getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& newSelection, const QItemSelection& oldSelection) {
        
        // Deactivate deselected layers
        if (!oldSelection.indexes().isEmpty()) {

            // Get pointer to layer that was deselected
            auto layer = static_cast<Layer*>(oldSelection.indexes().first().internalPointer());

            // Disconnect previously selected layer
            disconnect(layer, &Layer::selectionChanged, this, nullptr);
        }

        // Activate selected layers
        if (!newSelection.indexes().isEmpty()) {

            // Get pointer to layer that was selected
            auto layer = static_cast<Layer*>(newSelection.indexes().first().internalPointer());

            // Enable/disable
            const auto updateEnabled = [this, layer]() -> void {
                setEnabled(!layer->getSelectedIndices().empty());
            };

            // Enable/disable when the layer selection changes
            connect(layer, &Layer::selectionChanged, this, [this](const std::vector<std::uint32_t>& selectedIndices) {
                setEnabled(!selectedIndices.empty());
            });

            // Do an initial update when the layer is selected
            updateEnabled();
        }
    });

    // Show the create subset dialog when clicked
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Create subset dialog
        CreateSubsetDialog createSubsetDialog(_imageViewerPlugin);

        // Show the dialog
        createSubsetDialog.exec();
    });
}
