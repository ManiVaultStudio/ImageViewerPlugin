#pragma once

#include <actions/GroupAction.h>
#include <actions/DecimalAction.h>
#include <actions/ColorMap1DAction.h>
#include <actions/ColorMap2DAction.h>
#include <actions/ToggleAction.h>
#include <actions/ColorAction.h>

#include "ScalarChannelAction.h"

#include <QTimer>

class Layer;

using namespace mv::gui;

/**
 * Image settings action class
 *
 * Action class for layer image settings
 *
 * @author Thomas Kroes
 */
class ImageSettingsAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE ImageSettingsAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p layer
     * @param layer Pointer to owning layer
     */
    void initialize(Layer* layer);

    /** Get the number of active scalar channels */
    const std::uint32_t getNumberOfActiveScalarChannels() const;

protected: // Color map

    /** Get color map image */
    QImage getColorMapImage() const;

    /** Update the color map image and notify others */
    void updateColorMapImage();

protected:

    /** Invoked when the color space changed */
    void colorSpaceChanged();

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

    DecimalAction& getOpacityAction() { return _opacityAction; }
    IntegralAction& getSubsampleFactorAction() { return _subsampleFactorAction; }
    OptionAction& getColorSpaceAction() { return _colorSpaceAction; }
    ScalarChannelAction& getScalarChannel1Action() { return _scalarChannel1Action; }
    ScalarChannelAction& getScalarChannel2Action() { return _scalarChannel2Action; }
    ScalarChannelAction& getScalarChannel3Action() { return _scalarChannel3Action; }
    ColorMapAction& getColorMap1DAction() { return _colorMap1DAction; }
    ColorMapAction& getColorMap2DAction() { return _colorMap2DAction; }
    OptionAction& getInterpolationTypeAction() { return _interpolationTypeAction; }
    ToggleAction& getUseConstantColorAction() { return _useConstantColorAction; }
    ToggleAction& getFixChannelRangesToColorSpaceAction() { return _fixChannelRangesToColorSpaceAction; }
    ColorAction& getConstantColorAction() { return _constantColorAction; }

signals:

    /** Signals the layer image changed */
    void changed();

    /**
     * Signals the scalar channel changed
     * @param scalarChannelAction Reference to scalar channel action that changed
     */
    void channelChanged(ScalarChannelAction& scalarChannelAction);

protected:
    Layer*                  _layer;                                 /** Reference to layer */
    DecimalAction           _opacityAction;                         /** Opacity action */
    IntegralAction          _subsampleFactorAction;                 /** Subsample factor action */
    OptionAction            _colorSpaceAction;                      /** Color space action */
    ScalarChannelAction     _scalarChannel1Action;                  /** Scalar channel 1 action */
    ScalarChannelAction     _scalarChannel2Action;                  /** Scalar channel 2 action */
    ScalarChannelAction     _scalarChannel3Action;                  /** Scalar channel 3 action */
    ColorMap1DAction        _colorMap1DAction;                      /** One-dimensional color map action */
    ColorMap2DAction        _colorMap2DAction;                      /** Two-dimensional color map action */
    OptionAction            _interpolationTypeAction;               /** Interpolation type action */
    ToggleAction            _useConstantColorAction;                /** Constant color action */
    ToggleAction            _fixChannelRangesToColorSpaceAction;    /** Fixes ranges of channels to color space ranges action */
    ColorAction             _constantColorAction;                   /** Color action */
    QTimer                  _updateSelectionTimer;                  /** Timer to update layer selection when appropriate */
    QTimer                  _updateScalarDataTimer;                 /** Timer to update layer scalar data when appropriate */

    static const std::int32_t LAZY_UPDATE_INTERVAL = 0;
};

Q_DECLARE_METATYPE(ImageSettingsAction)

inline const auto imageSettingsActionMetaTypeId = qRegisterMetaType<ImageSettingsAction*>("ImageSettingsAction");
