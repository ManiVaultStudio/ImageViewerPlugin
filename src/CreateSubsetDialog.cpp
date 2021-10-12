#include "CreateSubsetDialog.h"
#include "ImageViewerPlugin.h"
#include "Application.h"

#include "ImageData/Images.h"

#include <QDebug>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>

#include <stdexcept>

using namespace hdps;

CreateSubsetDialog::CreateSubsetDialog(ImageViewerPlugin& imageViewerPlugin) :
    QDialog(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _nameAction(this, "Name"),
    _fromRegionAction(this, "From region", true, true)
{
    // Update window title and icon
    setWindowTitle("Create subset");
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("crop"));

    _nameAction.setToolTip("Name of the subset");
    _fromRegionAction.setToolTip("Create an image from the selected region");

    // Actions may not be reset
    _fromRegionAction.setMayReset(false);
    _nameAction.setMayReset(false);

    auto layout = new QGridLayout();

    // Add the widget to the layout
    layout->addWidget(_nameAction.createLabelWidget(this), 0, 0);
    layout->addWidget(_nameAction.createWidget(this), 0, 1);
    layout->addWidget(_fromRegionAction.createWidget(this), 1, 1);

    setLayout(layout);

    // Create dialog button box so that the user can proceed or cancel with the conversion
    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("&Create");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("C&ancel");

    // Add buttons to the layout
    layout->addWidget(dialogButtonBox, 2, 0, 1, 2);

    // Update the state of the create button
    const auto updateCreateButton = [this, dialogButtonBox]() {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!_nameAction.getString().isEmpty());
    };

    // Update the state of the create button when the name changes
    connect(&_nameAction, &StringAction::stringChanged, this, updateCreateButton);

    // Do an initial update
    updateCreateButton();

    // Handle when accepted
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, [this]() {
        try {

            // Get selected row from selection model
            const auto selectedRows = _imageViewerPlugin.getSelectionModel().selectedRows();

            // Only accept one selected layer at a time
            if (selectedRows.isEmpty())
                throw std::runtime_error("No layer selected");

            // Get pointer to selected layer
            auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

            auto& points = layer->getPoints();
            auto& images = layer->getImages();

            if (_fromRegionAction.isChecked()) {

                // Get the image size
                const auto imageSize = layer->getImageSize();

                // Cache the selection indices
                auto cachedSelectionIndices = layer->getSelectedIndices();

                // Get the selection boundaries
                const auto selectionBoundaries = layer->getSelectionAction().getSelectionBoundaries();

                // Compute the number of pixels in the region
                const auto numberOfPixelsInRegion = selectionBoundaries.width() * selectionBoundaries.height();

                // Get reference to selection indices
                auto& selectionIndices = dynamic_cast<Points&>(points.getSourceData().getSelection()).indices;

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
                DatasetRef<Points> pointsSubset(points.getSourceData().createSubset(_nameAction.getString(), points->getName()));

                // Notify that the points set was added
                Application::core()->notifyDataAdded(pointsSubset.getDatasetName());

                // Reset selected indices
                selectionIndices = cachedSelectionIndices;

                // Create a new image dataset which is a subset of the original image
                DatasetRef<Images> imagesSubset(Application::core()->addData("Images", _nameAction.getString(), pointsSubset.getDatasetName()));

                imagesSubset->setType(images->getType());
                imagesSubset->setNumberOfImages(images->getNumberOfImages());
                imagesSubset->setImageGeometry(images->getSourceRectangle().size(), selectionBoundaries.size(), selectionBoundaries.topLeft());
                imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());
                //imagesSubset->setImageFilePaths(images->getImag);

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

        // Exit the dialog
        accept();
    });

    // Handle when rejected
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &CreateSubsetDialog::reject);
}
