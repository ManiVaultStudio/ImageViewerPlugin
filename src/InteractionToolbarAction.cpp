#include "InteractionToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

#include <QtNumeric>

using namespace mv::util;

const float InteractionToolbarAction::zoomDeltaPercentage = 0.1f;

InteractionToolbarAction::InteractionToolbarAction(QObject* parent, const QString& title) :
    HorizontalToolbarAction(parent, title, Qt::AlignCenter),
    _imageViewerPlugin(nullptr),
    _navigationAction(this, "Navigation"),
    _selectAction(this, "Select"),
    _interactionModeAction(this, "Interaction Mode", { "Navigation", "Select" }, "Navigation"),
    _interactionModeGroupAction(this, "Interaction"),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 10.0f, 1000.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Around Selection"),
    _updateSelectionROI(this, "Update Selection ROI"),
    _zoomGroupAction(this, "Zoom"),
    _viewSettingsAction(this, "View Settings")
{
    _navigationAction.setToolTip("Navigate the view");
    _selectAction.setToolTip("Select pixels");
    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers (z)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the selection (d)");
    _updateSelectionROI.setToolTip("Update selection boundaries (u)");

    _navigationAction.setIconByName("hand-pointer");
    _selectAction.setIconByName("mouse-pointer");
    _interactionModeGroupAction.setIconByName("hand-sparkles");

    _zoomOutAction.setIconByName("search-minus");
    _zoomInAction.setIconByName("search-plus");
    _zoomExtentsAction.setIconByName("compress");
    _zoomSelectionAction.setIconByName("search-location");
    _updateSelectionROI.setIconByName("search-location");
    _zoomGroupAction.setIconByName("search");
    
    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _zoomSelectionAction.setShortcut(QKeySequence("d"));
    _updateSelectionROI.setShortcut(QKeySequence("u"));

    _selectAction.setEnabled(false);
    _zoomSelectionAction.setEnabled(false);
    _updateSelectionROI.setEnabled(false);

    _interactionModeAction.setIconByName("hand-sparkles");
    _interactionModeAction.setToolTip("Interaction Mode");

    _interactionModeGroupAction.setDefaultWidgetFlags(GroupAction::Horizontal);
    _interactionModeGroupAction.setShowLabels(false);

    _interactionModeGroupAction.addAction(&_navigationAction, ToggleAction::PushButtonIcon);
    _interactionModeGroupAction.addAction(&_selectAction, ToggleAction::PushButtonIcon);

    _zoomPercentageAction.setSuffix("%");
    _zoomPercentageAction.setUpdateDuringDrag(false);

    _zoomGroupAction.setDefaultWidgetFlags(GroupAction::Horizontal);
    _zoomGroupAction.setShowLabels(false);

    _zoomGroupAction.addAction(&_zoomOutAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomPercentageAction);
    _zoomGroupAction.addAction(&_zoomInAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomExtentsAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_zoomSelectionAction, TriggerAction::Icon);
    _zoomGroupAction.addAction(&_updateSelectionROI, TriggerAction::Icon);

    _viewSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);

    addAction(&_interactionModeGroupAction, 1, GroupAction::Horizontal);
    addAction(&_zoomGroupAction, 2, GroupAction::Horizontal);
    addAction(&_viewSettingsAction);
}

void InteractionToolbarAction::initialize(ImageViewerPlugin* imageViewerPlugin)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;
    
    getImageViewerWidget().addAction(&_zoomOutAction);
    getImageViewerWidget().addAction(&_zoomInAction);
    getImageViewerWidget().addAction(&_zoomExtentsAction);
    getImageViewerWidget().addAction(&_zoomSelectionAction);
    getImageViewerWidget().addAction(&_updateSelectionROI);

    _viewSettingsAction.initialize(_imageViewerPlugin);

    connect(&_navigationAction, &ToggleAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _interactionModeAction.setCurrentText("Navigation");
    });

    connect(&_selectAction, &ToggleAction::toggled, this, [this](bool toggled) {
        if (toggled)
            _interactionModeAction.setCurrentText("Select");
    });

    const auto currentIndexChanged = [this]() -> void {
        const auto currentIndex = _interactionModeAction.getCurrentIndex();

        _navigationAction.setChecked(currentIndex == 0);
        _selectAction.setChecked(currentIndex == 1);

        getImageViewerWidget().setInteractionMode(currentIndex == 0 ? ImageViewerWidget::Navigation : ImageViewerWidget::Selection);
    };

    currentIndexChanged();

    connect(&_interactionModeAction, &OptionAction::currentIndexChanged, this, currentIndexChanged);

    const auto updateZoomPercentage = [this]() -> void {
        const auto zoomPercentage = 100.0f * getImageViewerWidget().getRenderer().getZoomPercentage();

        if(qAbs(_zoomPercentageAction.getValue() - zoomPercentage) < 1e-5f)
            return;

        _zoomOutAction.setEnabled(zoomPercentage > _zoomPercentageAction.getMinimum());
        _zoomPercentageAction.setValue(zoomPercentage);
        _zoomInAction.setEnabled(zoomPercentage < _zoomPercentageAction.getMaximum());
    };

    updateZoomPercentage();

    auto triggerUpdateZoomPercentageAfterAnimation = [this, updateZoomPercentage]() -> void {
        connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, this, [this, updateZoomPercentage]() {
            updateZoomPercentage();
            disconnect(&getImageViewerWidget().getRenderer(), &LayersRenderer::animationFinished, nullptr, nullptr);
        });
    };

    connect(&_zoomOutAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() - zoomDeltaPercentage);
        getImageViewerWidget().update();
        emit getImageViewerWidget().navigationEnded();
    });

    connect(&_zoomPercentageAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        if (qAbs(_zoomPercentageAction.getValue() - 0.01f * value) < 1e-5f)
            return;

        getImageViewerWidget().getRenderer().setZoomPercentage(0.01f * value);
        getImageViewerWidget().update();
        emit getImageViewerWidget().navigationEnded();
    });

    connect(&_zoomInAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() + zoomDeltaPercentage);
        getImageViewerWidget().update();
        emit getImageViewerWidget().navigationEnded();
    });

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        getImageViewerWidget().getRenderer().setZoomRectangle(worldBoundingRectangle);
        getImageViewerWidget().update();
        emit getImageViewerWidget().navigationEnded();

        triggerUpdateZoomPercentageAfterAnimation();
    });

    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto selectedRows = _imageViewerPlugin->getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = _imageViewerPlugin->getLayersModel().getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return;

        layer->zoomToSelection();

        triggerUpdateZoomPercentageAfterAnimation();
    });

    connect(&_updateSelectionROI, &TriggerAction::triggered, this, [this]() {
        const auto selectedRows = _imageViewerPlugin->getSelectionModel().selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = _imageViewerPlugin->getLayersModel().getLayerFromIndex(selectedRows.first());

        Q_ASSERT(layer != nullptr);

        if (layer == nullptr)
            return;

        layer->updateSelectionROIMiscAction();

    });

    connect(&_imageViewerPlugin->getSelectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection& newSelection, const QItemSelection& oldSelection) {
        if (!oldSelection.indexes().isEmpty()) {
            auto layer = _imageViewerPlugin->getLayersModel().getLayerFromIndex(oldSelection.indexes().first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

            disconnect(layer, &Layer::selectionChanged, this, nullptr);
        }

        if (!newSelection.indexes().isEmpty()) {
            auto layer = _imageViewerPlugin->getLayersModel().getLayerFromIndex(newSelection.indexes().first());

            Q_ASSERT(layer != nullptr);

            if (layer == nullptr)
                return;

            connect(layer, &Layer::selectionChanged, this, [this](const std::vector<std::uint32_t>& selectedIndices) -> void {
                _zoomSelectionAction.setEnabled(!selectedIndices.empty());
                _updateSelectionROI.setEnabled(!selectedIndices.empty());
            });

            _selectAction.setEnabled(true);
            _zoomSelectionAction.setEnabled(!layer->getSelectedIndices().empty());
            _updateSelectionROI.setEnabled(!layer->getSelectedIndices().empty());
        }
        else {
            _selectAction.setEnabled(false);
            _zoomSelectionAction.setEnabled(false);
            _updateSelectionROI.setEnabled(false);
        }
    });

    // Hotfix: this interferes with setting a zoomed in ROI from a connected rectangle action
    // the zoom factor is not updated an this call reverts to the old zoom factor, changin the ROI again
    //connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::zoomRectangleChanged, this, [this, updateZoomPercentage]() {
    //    updateZoomPercentage();
    //});
}

ImageViewerWidget& InteractionToolbarAction::getImageViewerWidget()
{
    return _imageViewerPlugin->getImageViewerWidget();
}

void InteractionToolbarAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalToolbarAction::fromVariantMap(variantMap);

    _interactionModeAction.fromParentVariantMap(variantMap);
    _viewSettingsAction.fromParentVariantMap(variantMap);
}

QVariantMap InteractionToolbarAction::toVariantMap() const
{
    auto variantMap = HorizontalToolbarAction::toVariantMap();

    _interactionModeAction.insertIntoVariantMap(variantMap);
    _viewSettingsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}