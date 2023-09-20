#pragma once

#include <actions/GroupAction.h>
#include <actions/ToggleAction.h>
#include <actions/OptionAction.h>
#include <actions/WindowLevelAction.h>

#include <ImageData/Images.h>

using namespace hdps::gui;
using namespace hdps::util;

class Layer;

/**
 * Scalar channel action class
 *
 * Layer channel class
 *
 * @author Thomas Kroes
 */
class ScalarChannelAction : public GroupAction
{
    Q_OBJECT

public:

    /** Channel identifier enumerations */
    enum Identifier {
        Channel1,       /** Channel 1 */
        Channel2,       /** Channel 2 */
        Channel3,       /** Channel 3 */

        Count = Channel3 + 1
    };

    /** Maps channel index enum to name */
    static const QMap<Identifier, QString> channelIndexes;

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title
     */
    Q_INVOKABLE ScalarChannelAction(QObject* parent, const QString& title);

    /**
     * Initialize with \p layer, channel \p identifier
     * @param layer Pointer to layer
     * @param identifier Channel index identifier
     */
    void initialize(Layer* layer, const Identifier& identifier);

    /** Get the channel identifier */
    const Identifier getIdentifier() const;

    /** Get image size */
    QSize getImageSize();

    /** Set fixed displayRange */
    void setFixedDisplayRange(bool status, float lower = -1, float upper = -1);

public: // Channel data

    /** Get scalar data */
    const QVector<float>& getScalarData() const;

    /** Get scalar data range */
    const QPair<float, float>& getScalarDataRange() const;

    /** Get display range */
    QPair<float, float> getDisplayRange();

    /** Compute scalar data for image sequence */
    void computeScalarData();

protected:

    /** Get smart pointer to images dataset */
    hdps::Dataset<Images> getImages();

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
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /** Signals the channel changed */
    void changed(ScalarChannelAction& channelAction);

public: // Action getters

    OptionAction& getDimensionAction() { return _dimensionAction; }
    ToggleAction& getEnabledAction() { return _enabledAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

private:
    Layer*                  _layer;                 /** Pointer to layer */
    Identifier              _identifier;            /** Channel index */
    ToggleAction            _enabledAction;         /** Enabled action */
    OptionAction            _dimensionAction;       /** Selected dimension action */
    WindowLevelAction       _windowLevelAction;     /** Window/level action */
    QVector<float>          _scalarData;            /** Channel scalar data for the specified dimension */
    QPair<float, float>     _scalarDataRange;       /** Scalar data range */
    QPair<float, float>     _displayRange;
    bool                    _fixedDisplayRange;

    friend class ImageAction;
};

Q_DECLARE_METATYPE(ScalarChannelAction)

inline const auto scalarChannelActionMetaTypeId = qRegisterMetaType<ScalarChannelAction*>("ScalarChannelAction");