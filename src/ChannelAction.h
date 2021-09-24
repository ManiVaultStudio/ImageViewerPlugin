#pragma once

#include "WindowLevelAction.h"

#include "event/EventListener.h"

#include "actions/WidgetAction.h"
#include "actions/ToggleAction.h"
#include "actions/OptionAction.h"

#include "util/DatasetRef.h"

#include "ImageData/Images.h"

using namespace hdps::gui;
using namespace hdps::util;

class LayerImageAction;

/**
 * Channel action class
 *
 * Layer channel class
 *
 * @author Thomas Kroes
 */
class ChannelAction : public WidgetAction, public hdps::EventListener
{

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

public:

    /** Describes the widget flags */
    enum WidgetFlag {
        ComboBox            = 0x00002,  /** The widget includes a dimension selection combobox */
        WindowLevelWidget   = 0x00004,  /** The widget includes a window/level settings widget */
        ResetPushButton     = 0x00008,  /** The widget includes a reset push button */

        All = ComboBox | WindowLevelWidget | ResetPushButton
    };

public:

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param channelAction Pointer to channel action
         * @param state State of the widget
         */
        Widget(QWidget* parent, ChannelAction* channelAction, const WidgetActionWidget::State& state);

    protected:
        friend class ChannelAction;
    };

protected:

    /**
     * Get widget representation of the settings action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new Widget(parent, this, state);
    };

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset the current color to the default color */
    void reset() override;

public:

    /** 
     * Constructor
     * @param layerImageAction Reference to layer image action
     * @param index Channel index
     * @param name Name of the channel
     */
    ChannelAction(LayerImageAction& layerImageAction, const ChannelIndex& index, const QString& name);

private: // Data extraction

    /** Get reference to images dataset */
    DatasetRef<Images>& getImages();

    /** Get reference to points dataset which contains the actual image data */
    DatasetRef<Points>& getPoints();

    /** Get selection indices */
    std::vector<std::uint32_t> getSelectionIndices();

    /** Compute scalar data for image sequence */
    void computeScalarData();

    /** Compute scalar data for image sequence */
    void computeScalarDataForImageSequence();

    /** Compute scalar data for image stack */
    void computeScalarDataForImageStack();

    /** Compute mask channel */
    void computeMaskChannel();

    /** Compute selection channel */
    void computeSelectionChannel();

public: /** Action getters */

    OptionAction& getDimensionAction() { return _dimensionAction; }
    ToggleAction& getEnabledAction() { return _enabledAction; }
    WindowLevelAction& getWindowLevelAction() { return _windowLevelAction; }

protected:
    LayerImageAction&       _layerImageAction;      /** Reference to layer image action */
    const ChannelIndex      _index;                 /** Channel index */
    ToggleAction            _enabledAction;         /** Enabled action */
    OptionAction            _dimensionAction;       /** Selected dimension action */
    WindowLevelAction       _windowLevelAction;     /** Window/level action */
    QVector<float>          _scalarData;            /** Channel scalar data for the specified dimension */
};
