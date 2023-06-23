#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>
#include <actions/OptionAction.h>
#include <actions/HorizontalGroupAction.h>

#include "ViewSettingsAction.h"

class ImageViewerPlugin;
class ImageViewerWidget;

using namespace hdps::gui;

/**
 * Interaction toolbar action class
 *
 * Action class for various interaction modalities
 *
 * @author Thomas Kroes
 */
class InteractionToolbarAction : public HorizontalToolbarAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE InteractionToolbarAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p imageViewerPlugin
     * @param imageViewerPlugin Pointer to image viewer plugin
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin);

    /** Get reference to image viewer widget */
    ImageViewerWidget& getImageViewerWidget();

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ToggleAction& getNavigationAction() { return _navigationAction; }
    ToggleAction& getSelectAction() { return _selectAction; }
    OptionAction& getInteractionModeAction() { return _interactionModeAction; }
    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    TriggerAction& getZoomSelectionAction() { return _zoomSelectionAction; }
    ViewSettingsAction& getViewSettingsAction() { return _viewSettingsAction; }

protected:
    ImageViewerPlugin*      _imageViewerPlugin;             /** Pointer to image viewer plugin */
    ToggleAction            _navigationAction;              /** Navigation interaction mode action */
    ToggleAction            _selectAction;                  /** Select interaction mode action */
    OptionAction            _interactionModeAction;         /** Interaction mode option action */
    HorizontalGroupAction   _interactionModeGroupAction;    /** Interaction mode option action */
    TriggerAction           _zoomOutAction;                 /** Zoom out action */
    DecimalAction           _zoomPercentageAction;          /** Zoom action */
    TriggerAction           _zoomInAction;                  /** Zoom in action */
    TriggerAction           _zoomExtentsAction;             /** Zoom extents action */
    TriggerAction           _zoomSelectionAction;           /** Zoom to selection extents action */
    ViewSettingsAction      _viewSettingsAction;            /** View settings action */

    static const float zoomDeltaPercentage;
};

Q_DECLARE_METATYPE(InteractionToolbarAction)

inline const auto interactionToolbarActionMetaTypeId = qRegisterMetaType<InteractionToolbarAction*>("InteractionToolbarAction");