#pragma once

#include "actions/WidgetAction.h"
#include "actions/TriggerAction.h"
#include "actions/DecimalAction.h"
#include "actions/ToggleAction.h"

#include "SubsetAction.h"
#include "GlobalViewSettingsAction.h"
#include <QActionGroup>

class ImageViewerPlugin;
class ImageViewerWidget;

using namespace hdps::gui;

/**
 * Main toolbar action class
 *
 * Action class for main toolbar
 *
 * @author Thomas Kroes
 */
class MainToolbarAction : public WidgetAction
{
public:

    /** Widget class for main toolbar action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param mainToolbarAction Pointer to main toolbar action
         */
        Widget(QWidget* parent, MainToolbarAction* mainToolbarAction);

    protected:
        friend class MainToolbarAction;
    };

protected:

    /**
     * Get widget representation of the main toolbar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    MainToolbarAction(ImageViewerPlugin& imageViewerPlugin);

    /** Determines whether the current value can be reset to its default */
    bool isResettable() const override {
        return false;
    };

    /** Reset the current value to the default value */
    void reset() override {};

    /** Get reference to image viewer widget */
    ImageViewerWidget& getImageViewerWidget();

protected:

    /** Setup the interaction actions */
    void setupInteraction();

public: // Action getters

    ToggleAction& getPanAction() { return _panAction; }
    ToggleAction& getSelectAction() { return _selectAction; }
    ToggleAction& getRectangleSelectionAction() { return _rectangleSelectionAction; }
    ToggleAction& getBrushSelectionAction() { return _brushSelectionAction; }
    ToggleAction& getLassoSelectionAction() { return _lassoSelectionAction; }
    ToggleAction& getPolygonSelectionAction() { return _polygonSelectionAction; }
    ToggleAction& getSampleSelectionAction() { return _sampleSelectionAction; }
    ToggleAction& getRoiSelectionAction() { return _roiSelectionAction; }
    SubsetAction& getSubsetAction() { return _subsetAction; }
    TriggerAction& getExportToImageAction() { return _exportToImageAction; }
    GlobalViewSettingsAction& getGlobalViewSettingsAction() { return _globalViewSettingsAction; }

protected:
    ImageViewerPlugin&          _imageViewerPlugin;                 /** Reference to image viewer plugin */
    ToggleAction                _panAction;                         /** Pan interaction mode action */
    ToggleAction                _selectAction;                      /** Select interaction mode action */
    ToggleAction                _rectangleSelectionAction;          /** Rectangle selection action */
    ToggleAction                _brushSelectionAction;              /** Brush selection action */
    ToggleAction                _lassoSelectionAction;              /** Lasso selection action */
    ToggleAction                _polygonSelectionAction;            /** Polygon selection action */
    ToggleAction                _sampleSelectionAction;             /** Sample selection action */
    ToggleAction                _roiSelectionAction;                /** ROI selection action */
    SubsetAction                _subsetAction;                      /** Subset action */
    TriggerAction               _exportToImageAction;               /** Export to image action */
    QActionGroup                _interactionModeActionGroup;        /** Interaction mode action group */
    GlobalViewSettingsAction    _globalViewSettingsAction;          /** Global view settings action */
};
