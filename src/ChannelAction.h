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
 * Channel action class
 *
 * Layer channel class
 *
 * @author Thomas Kroes
 */
class ChannelAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Channel index enumerations */
    enum ChannelIndex {
        Channel1,       /** Channel 1 */
        Channel2,       /** Channel 2 */
        Channel3,       /** Channel 3 */
        Mask,           /** Mask channel */
        Selection,      /** Selection channel */

        Count = Selection + 1
    };

    /** Maps channel index enum to name */
    static const QMap<ChannelIndex, QString> channelIndexes;

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
     * @param state State of the widget (for stateful widgets)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset the current color to the default color */
    void reset() override;

public:

    /** 
     * Constructor
     * @param imageAction Reference to layer image action
     * @param index Channel index
     * @param name Name of the channel
     */
    ChannelAction(ImageAction& imageAction, const ChannelIndex& index, const QString& name);

    /** Get the channel index */
    const ChannelIndex getIndex() const;

    /** Get image size */
    QSize getImageSize();

public: // Channel data

    /** Get scalar data */
    const QVector<float>& getScalarData() const;

    /** Get scalar data range */
    const QPair<float, float>& getScalarDataRange() const;

    /** Get display range */
    QPair<float, float> getDisplayRange();

    /** Get selection data */
    const std::vector<std::uint8_t>& getSelectionData() const;

    /** Get selection boundaries */
    QRect getSelectionBoundaries() const;

protected: // Data extraction

    /** Get reference to images dataset */
    DatasetRef<Images>& getImages();

    /** Get reference to points dataset which contains the actual image data */
    DatasetRef<Points>& getPoints();

    /** Compute scalar data for image sequence */
    void computeScalarData();

    /** Compute mask channel */
    void computeMaskChannel();

    /** Compute selection channel */
    void computeSelectionChannel();

signals:
    
    /** Signals the channel changed */
    void changed(ChannelAction& channelAction);

public: /** Action getters */

    OptionAction& getDimensionAction() { return _dimensionAction; }
    ToggleAction& getEnabledAction() { return _enabledAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

protected:
    ImageAction&                _imageAction;               /** Reference to image action */
    const ChannelIndex          _index;                     /** Channel index */
    ToggleAction                _enabledAction;             /** Enabled action */
    OptionAction                _dimensionAction;           /** Selected dimension action */
    WindowLevelAction           _windowLevelAction;         /** Window/level action */
    QVector<float>              _scalarData;                /** Channel scalar data for the specified dimension */
    QPair<float, float>         _scalarDataRange;           /** Scalar data range */
    std::vector<std::uint8_t>   _selectionData;             /** Selection data */
    QRect                       _selectionBoundaries;       /** Selection boundaries */

    friend class ImageAction;
};
