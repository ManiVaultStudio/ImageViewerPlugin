#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include <Application.h>

#include <util/Exception.h>

#include <PointData/PointData.h>

using namespace hdps;

SubsetAction::SubsetAction(ImageViewerPlugin& imageViewerPlugin) :
    TriggerAction(&imageViewerPlugin),
    _imageViewerPlugin(imageViewerPlugin),
    _nameAction(this, "Name"),
    _createAction(this, "Create")
{
    setText("Create subset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("crop"));

    _nameAction.setToolTip("Name of the subset");
    _createAction.setToolTip("Create the subset");

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
            auto points = Dataset<Points>(layer->getSourceDataset());

            // Get reference to images dataset
            auto images = layer->getImages();

            // Create the points subset
            auto subset = points->createSubsetFromSelection(_nameAction.getString(), points);

            // Notify that the points set was added
            events().notifyDatasetAdded(subset);

            // Create a new image dataset which is a subset of the original image
            auto imagesSubset = Application::core()->addDataset<Images>("Images", _nameAction.getString(), subset);

            imagesSubset->setType(images->getType());
            imagesSubset->setNumberOfImages(images->getNumberOfImages());
            imagesSubset->setImageSize(images->getImageSize());
            imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());
            //imagesSubset->setImageFilePaths(images->getImag);

            // Notify others that the images dataset was added
            events().notifyDatasetAdded(*imagesSubset);
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

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, subsetAction),
    _subsetAction(subsetAction)
{
    auto layout = new QGridLayout();

    layout->addWidget(subsetAction->getNameAction().createLabelWidget(this), 0, 0);
    layout->addWidget(subsetAction->getNameAction().createWidget(this), 0, 1);
    layout->addWidget(subsetAction->getCreateAction().createWidget(this), 2, 1);

    // Close the widget when the create action is triggered
    connect(&subsetAction->getCreateAction(), &TriggerAction::triggered, this, [this]() {
        parentWidget()->close();
    });

    if (widgetFlags & PopupLayout)
    {
        setPopupLayout(layout);
    }
    else {
        layout->setContentsMargins(0, 0, 0, 0);
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
