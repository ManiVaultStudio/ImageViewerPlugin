#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include <util/Exception.h>

#include <PointData/PointData.h>

using namespace hdps;

SubsetAction::SubsetAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _imageViewerPlugin(nullptr),
    _nameAction(this, "Name"),
    _createAction(this, "Create")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("crop"));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    addAction(&_nameAction);
    addAction(&_createAction);

    _nameAction.setToolTip("Name of the subset");
    _createAction.setToolTip("Create the subset");

    const auto updateCreateButtonReadOnly = [this]() {
        _createAction.setEnabled(!_nameAction.getString().isEmpty());
    };

    updateCreateButtonReadOnly();

    connect(&_nameAction, &StringAction::stringChanged, this, updateCreateButtonReadOnly);
}

void SubsetAction::initialize(ImageViewerPlugin* imageViewerPlugin)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;

    connect(&_createAction, &TriggerAction::triggered, this, [this]() -> void {
        try {
            const auto selectedRows = _imageViewerPlugin->getSelectionModel().selectedRows();

            if (selectedRows.isEmpty())
                throw std::runtime_error("No layer selected");

            auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());
            auto points = Dataset<Points>(layer->getSourceDataset());
            auto images = layer->getImages();

            auto subset = points->createSubsetFromSelection(_nameAction.getString(), points);

            events().notifyDatasetAdded(subset);

            auto imagesSubset = Application::core()->addDataset<Images>("Images", _nameAction.getString(), subset);

            imagesSubset->setType(images->getType());
            imagesSubset->setNumberOfImages(images->getNumberOfImages());
            imagesSubset->setImageSize(images->getImageSize());
            imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());

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
