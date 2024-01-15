#include "SubsetAction.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"

#include <util/Exception.h>

#include <PointData/PointData.h>

using namespace mv;

SubsetAction::SubsetAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _imageViewerPlugin(nullptr),
    _nameAction(this, "Name"),
    _createAction(this, "Create")
{
    setIcon(Application::getIconFont("FontAwesome").getIcon("crop"));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setConnectionPermissionsToForceNone(true);

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

            auto layer = _imageViewerPlugin->getLayersModel().getLayerFromIndex(selectedRows.first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

            auto points = Dataset<Points>(layer->getSourceDataset());
            auto images = layer->getImagesDataset();

            auto subset = points->createSubsetFromSelection(_nameAction.getString(), points);

            auto imagesSubset = mv::data().createDataset<Images>("Images", _nameAction.getString(), subset);

            imagesSubset->setType(images->getType());
            imagesSubset->setNumberOfImages(images->getNumberOfImages());
            imagesSubset->setImageSize(images->getImageSize());
            imagesSubset->setNumberOfComponentsPerPixel(images->getNumberOfComponentsPerPixel());
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

void SubsetAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicSubsetAction = dynamic_cast<SubsetAction*>(publicAction);

    Q_ASSERT(publicSubsetAction != nullptr);

    if (publicSubsetAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_nameAction, &publicSubsetAction->getNameAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_createAction, &publicSubsetAction->getCreateAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void SubsetAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_nameAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_createAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void SubsetAction::fromVariantMap(const QVariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _nameAction.fromParentVariantMap(variantMap);
    _createAction.fromParentVariantMap(variantMap);
}

QVariantMap SubsetAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _nameAction.insertIntoVariantMap(variantMap);
    _createAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
