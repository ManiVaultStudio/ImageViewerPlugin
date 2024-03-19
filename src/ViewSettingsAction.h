#pragma once

#include <actions/GroupAction.h>
#include <actions/DecimalAction.h>
#include <actions/ColorAction.h>
#include <actions/ToggleAction.h>

using namespace mv::gui;

class ImageViewerPlugin;

/**
 * Global view settings action class
 *
 * Settings action class for mangaing global view settings
 *
 * @author Thomas Kroes
 */
class ViewSettingsAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE ViewSettingsAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p imageViewerPlugin
     * @param imageViewerPlugin Pointer to image viewer plugin
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin);

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const mv::VariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    mv::VariantMap toVariantMap() const override;

public: // Action getters

    DecimalAction& getZoomMarginAction() { return _zoomMarginAction; }
    ColorAction& getBackgroundColorAction() { return _backgroundColorAction; }
    ToggleAction& getAnimationEnabledAction() { return _animationEnabledAction; }
    ToggleAction& getSmartZoomAction() { return _smartZoomAction; }

protected:
    ImageViewerPlugin*  _imageViewerPlugin;         /** Reference to image viewer plugin */
    DecimalAction       _zoomMarginAction;          /** Margin around layers extents action */
    ColorAction         _backgroundColorAction;     /** Background color action action */
    ToggleAction        _animationEnabledAction;    /** Animation on/off action */
    ToggleAction        _smartZoomAction;            /** Automatically zoom when selecting layers action */
};

Q_DECLARE_METATYPE(ViewSettingsAction)

inline const auto viewSettingsActionMetaTypeId = qRegisterMetaType<ViewSettingsAction*>("ViewSettingsAction");
