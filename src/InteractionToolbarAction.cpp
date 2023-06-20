#include "InteractionToolbarAction.h"
#include "ImageViewerPlugin.h"
#include "ImageViewerWidget.h"
#include "LayersModel.h"

using namespace hdps::util;

const float InteractionToolbarAction::zoomDeltaPercentage = 0.1f;

InteractionToolbarAction::InteractionToolbarAction(QObject* parent, const QString& title) :
    HorizontalToolbarAction(parent, title, Qt::AlignCenter),
    _imageViewerPlugin(nullptr),
    _navigationAction(this, "Navigation"),
    _selectAction(this, "Select"),
    _interactionModeAction(this, "Interaction Mode", { "Navigation", "Select" }, "Navigation"),
    _interactionModeGroupAction(this, "Interaction Mode Group"),
    _zoomOutAction(this, "Zoom out"),
    _zoomPercentageAction(this, "Zoom Percentage", 10.0f, 1000.0f, 100.0f, 1),
    _zoomInAction(this, "Zoom In"),
    _zoomExtentsAction(this, "Zoom All"),
    _zoomSelectionAction(this, "Zoom Around Selection"),
    _viewSettingsAction(this, "View Settings")
{
    addAction(&_interactionModeGroupAction);
    addAction(&_zoomOutAction, TriggerAction::Icon);
    addAction(&_zoomPercentageAction, IntegralAction::Slider);
    addAction(&_zoomInAction, TriggerAction::Icon);
    addAction(&_zoomExtentsAction, TriggerAction::Icon);
    addAction(&_zoomSelectionAction, TriggerAction::Icon);
    addAction(&_viewSettingsAction, TriggerAction::Icon);

    auto& fontAwesome = hdps::Application::getIconFont("FontAwesome");

    _navigationAction.setToolTip("Navigate the view");
    _selectAction.setToolTip("Select pixels");
    _zoomOutAction.setToolTip("Zoom out by 10% (-)");
    _zoomPercentageAction.setToolTip("Zoom in/out (+)");
    _zoomInAction.setToolTip("Zoom in by 10%");
    _zoomExtentsAction.setToolTip("Zoom to the boundaries of all layers (z)");
    _zoomSelectionAction.setToolTip("Zoom to the boundaries of the selection (d)");

    _navigationAction.setIcon(fontAwesome.getIcon("arrows-alt"));
    _selectAction.setIcon(fontAwesome.getIcon("mouse-pointer"));
    _zoomOutAction.setIcon(fontAwesome.getIcon("search-minus"));
    _zoomInAction.setIcon(fontAwesome.getIcon("search-plus"));
    _zoomExtentsAction.setIcon(fontAwesome.getIcon("compress"));
    _zoomSelectionAction.setIcon(fontAwesome.getIcon("search-plus"));
    
    _zoomOutAction.setShortcut(QKeySequence("-"));
    _zoomInAction.setShortcut(QKeySequence("+"));
    _zoomExtentsAction.setShortcut(QKeySequence("z"));
    _zoomSelectionAction.setShortcut(QKeySequence("d"));

    _interactionModeAction.setIcon(fontAwesome.getIcon("hand-sparkles"));
    _interactionModeAction.setToolTip("Interaction Mode");

    _interactionModeGroupAction.addAction(&_navigationAction, ToggleAction::PushButtonIcon);
    _interactionModeGroupAction.addAction(&_selectAction, ToggleAction::PushButtonIcon);

    _zoomPercentageAction.setSuffix("%");
    _zoomPercentageAction.setUpdateDuringDrag(false);

    _viewSettingsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
}

void InteractionToolbarAction::initialize(ImageViewerPlugin* imageViewerPlugin)
{
    Q_ASSERT(imageViewerPlugin != nullptr);

    if (imageViewerPlugin == nullptr)
        return;

    _imageViewerPlugin = imageViewerPlugin;
    
    getImageViewerWidget().addAction(&_navigationAction);
    getImageViewerWidget().addAction(&_selectAction);
    getImageViewerWidget().addAction(&_zoomOutAction);
    getImageViewerWidget().addAction(&_zoomInAction);
    getImageViewerWidget().addAction(&_zoomExtentsAction);
    getImageViewerWidget().addAction(&_zoomSelectionAction);

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
    });

    connect(&_zoomPercentageAction, &DecimalAction::valueChanged, this, [this](const float& value) {
        getImageViewerWidget().getRenderer().setZoomPercentage(0.01f * value);
        getImageViewerWidget().update();
    });

    connect(&_zoomInAction, &TriggerAction::triggered, this, [this]() {
        getImageViewerWidget().getRenderer().setZoomPercentage(getImageViewerWidget().getRenderer().getZoomPercentage() + zoomDeltaPercentage);
        getImageViewerWidget().update();
    });

    connect(&_zoomExtentsAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        getImageViewerWidget().getRenderer().setZoomRectangle(worldBoundingRectangle);
        getImageViewerWidget().update();

        triggerUpdateZoomPercentageAfterAnimation();
    });

    connect(&_zoomSelectionAction, &TriggerAction::triggered, this, [this, triggerUpdateZoomPercentageAfterAnimation]() {
        const auto worldBoundingRectangle = getImageViewerWidget().getWorldBoundingRectangle();

        auto& _selectionModel = _imageViewerPlugin->getSelectionModel();

        const auto selectedRows = _selectionModel.selectedRows();

        if (selectedRows.count() != 1)
            return;

        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        layer->zoomToSelection();
        layer->selectNone();

        triggerUpdateZoomPercentageAfterAnimation();
    });

    connect(&getImageViewerWidget().getRenderer(), &LayersRenderer::zoomRectangleChanged, this, [this, updateZoomPercentage]() {
        updateZoomPercentage();
    });
}

ImageViewerWidget& InteractionToolbarAction::getImageViewerWidget()
{
    return _imageViewerPlugin->getImageViewerWidget();
}

void InteractionToolbarAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalToolbarAction::fromVariantMap(variantMap);

    _interactionModeAction.fromParentVariantMap(variantMap);
}

QVariantMap InteractionToolbarAction::toVariantMap() const
{
    auto variantMap = HorizontalToolbarAction::toVariantMap();

    _interactionModeAction.insertIntoVariantMap(variantMap);

    return variantMap;
}