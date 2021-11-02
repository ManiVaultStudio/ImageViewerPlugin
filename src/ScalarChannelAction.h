#pragma once

#include "actions/WidgetAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"
#include "actions/WindowLevelAction.h"

#include "util/DatasetRef.h"

#include "ImageData/Images.h"

using namespace hdps::gui;
using namespace hdps::util;

class ImageAction;

/**
 * Scalar channel action class
 *
 * Layer channel class
 *
 * @author Thomas Kroes
 */
class ScalarChannelAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Channel identifier enumerations */
    enum Identifier {
        Channel1,       /** Channel 1 */
        Channel2,       /** Channel 2 */
        Channel3,       /** Channel 3 */
        Mask,           /** Mask channel */

        Count = Mask + 1
    };

    /** Maps channel index enum to name */
    static const QMap<Identifier, QString> channelIndexes;

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox            = 0x00001,  /** The widget includes a dimension selection combobox */
        WindowLevelWidget   = 0x00002,  /** The widget includes a window/level settings widget */
        ResetPushButton     = 0x00004,  /** The widget includes a reset push button */

        All = ComboBox | WindowLevelWidget | ResetPushButton
    };

protected:

    /**
     * Get widget representation of the channel action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset the current color to the default color */
    void reset() override;

public:

    /** 
     * Constructor
     * @param imageAction Reference to layer image action
     * @param index Channel index
     * @param name Name of the scalar channel
     */
    ScalarChannelAction(ImageAction& imageAction, const Identifier& index, const QString& name);

    /** Get the channel identifier */
    const Identifier getIdentifier() const;

    /** Get image size */
    QSize getImageSize();

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

    /** Compute mask channel */
    void computeMaskChannel();

    /** Get reference to images dataset */
    DatasetRef<Images>& getImages();

signals:
    
    /** Signals the channel changed */
    void changed(ScalarChannelAction& channelAction);

public: /** Action getters */

    OptionAction& getDimensionAction() { return _dimensionAction; }
    ToggleAction& getEnabledAction() { return _enabledAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

protected:
    ImageAction&            _imageAction;           /** Reference to image action */
    const Identifier        _identifier;            /** Channel index */
    ToggleAction            _enabledAction;         /** Enabled action */
    OptionAction            _dimensionAction;       /** Selected dimension action */
    WindowLevelAction       _windowLevelAction;     /** Window/level action */
    QVector<float>          _scalarData;            /** Channel scalar data for the specified dimension */
    QPair<float, float>     _scalarDataRange;       /** Scalar data range */

    friend class ImageAction;
};