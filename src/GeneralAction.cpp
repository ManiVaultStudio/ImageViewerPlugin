#include "GeneralAction.h"
#include "Layer.h"
#include "EditLayersAction.h"
#include "ImageViewerPlugin.h"

using namespace mv;

GeneralAction::GeneralAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _layer(nullptr),
    _visibleAction(this, "Visible", true),
    _datasetNameAction(this, "Dataset name"),
    _colorAction(this, "Color"),
    _nameAction(this, "Name"),
    _positionAction(this, "Position"),
    _scaleAction(this, "Scale", 0.0f, 1000000.0f, 100.0f, 1)
{
    addAction(&_visibleAction);
    addAction(&_datasetNameAction);
    addAction(&_colorAction);
    addAction(&_nameAction);
    addAction(&_positionAction);
    addAction(&_scaleAction);

    _datasetNameAction.setConnectionPermissionsToForceNone();
    _datasetNameAction.setEnabled(false);
    
    _nameAction.setConnectionPermissionsToForceNone();

    _scaleAction.setDefaultWidgetFlags(DecimalAction::SpinBox);

    _visibleAction.setToolTip("Visibility of the layer");
    _datasetNameAction.setToolTip("Name of the images dataset");
    _nameAction.setToolTip("Name of the layer");
    _scaleAction.setToolTip("Layer scale in percentages");

    _scaleAction.setSuffix("%");
}

void GeneralAction::initialize(Layer* layer)
{
    Q_ASSERT(layer != nullptr);

    if (layer == nullptr)
        return;

    _layer = layer;

    const auto layerColor = EditLayersAction::getRandomLayerColor();

    _colorAction.setColor(layerColor);
    _nameAction.setString(_layer->getImagesDataset()->text());

    const auto render = [this]() {
        _layer->getImageViewerPlugin().getImageViewerWidget().update();
    };

    render();

    const auto updateBounds = [this]() {
        _layer->getImageViewerPlugin().getImageViewerWidget().updateWorldBoundingRectangle();
    };

    updateBounds();

    connect(&_nameAction, &StringAction::stringChanged, this, render);
    connect(&_visibleAction, &ToggleAction::toggled, this, updateBounds);
    connect(&_positionAction, &PositionAction::changed, this, updateBounds);
    connect(&_scaleAction, &DecimalAction::valueChanged, this, updateBounds);
    connect(&_colorAction, &ColorAction::colorChanged, this, updateBounds);
    connect(&_colorAction, &ColorAction::colorChanged, this, render);

    const auto updateDatasetNameAction = [this]() -> void {
        _datasetNameAction.setString(_layer->getImagesDataset()->getDataHierarchyItem().getLocation());
    };

    updateDatasetNameAction();

    connect(_layer->getImagesDataset().get(), &DatasetImpl::locationChanged, this, updateDatasetNameAction);
}

void GeneralAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicGeneralAction = dynamic_cast<GeneralAction*>(publicAction);

    Q_ASSERT(publicGeneralAction != nullptr);

    if (publicGeneralAction == nullptr)
        return;

    if (recursive) {
        actions().connectPrivateActionToPublicAction(&_visibleAction, &publicGeneralAction->getVisibleAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_colorAction, &publicGeneralAction->getColorAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_nameAction, &publicGeneralAction->getNameAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_positionAction, &publicGeneralAction->getPositionAction(), recursive);
        actions().connectPrivateActionToPublicAction(&_scaleAction, &publicGeneralAction->getScaleAction(), recursive);
    }

    GroupAction::connectToPublicAction(publicAction, recursive);
}

void GeneralAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    if (recursive) {
        actions().disconnectPrivateActionFromPublicAction(&_visibleAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_colorAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_nameAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_positionAction, recursive);
        actions().disconnectPrivateActionFromPublicAction(&_scaleAction, recursive);
    }

    GroupAction::disconnectFromPublicAction(recursive);
}

void GeneralAction::fromVariantMap(const mv::VariantMap& variantMap)
{
    GroupAction::fromVariantMap(variantMap);

    _visibleAction.fromParentVariantMap(variantMap);
    _colorAction.fromParentVariantMap(variantMap);
    _nameAction.fromParentVariantMap(variantMap);
    _positionAction.fromParentVariantMap(variantMap);
    _scaleAction.fromParentVariantMap(variantMap);
}

mv::VariantMap GeneralAction::toVariantMap() const
{
    auto variantMap = GroupAction::toVariantMap();

    _visibleAction.insertIntoVariantMap(variantMap);
    _colorAction.insertIntoVariantMap(variantMap);
    _nameAction.insertIntoVariantMap(variantMap);
    _positionAction.insertIntoVariantMap(variantMap);
    _scaleAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
