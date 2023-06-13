#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>

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
class MainToolbarAction : public HorizontalToolbarAction
{
public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    MainToolbarAction(ImageViewerPlugin& imageViewerPlugin);

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
    ViewSettingsAction& getGlobalViewSettingsAction() { return _viewSettingsAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;                 /** Reference to image viewer plugin */
    ToggleAction            _panAction;                         /** Pan interaction mode action */
    ToggleAction            _selectAction;                      /** Select interaction mode action */
    ToggleAction            _rectangleSelectionAction;          /** Rectangle selection action */
    ToggleAction            _brushSelectionAction;              /** Brush selection action */
    ToggleAction            _lassoSelectionAction;              /** Lasso selection action */
    ToggleAction            _polygonSelectionAction;            /** Polygon selection action */
    ToggleAction            _sampleSelectionAction;             /** Sample selection action */
    ToggleAction            _roiSelectionAction;                /** ROI selection action */
    SubsetAction            _subsetAction;                      /** Subset action */
    TriggerAction           _exportToImageAction;               /** Export to image action */
    QActionGroup            _interactionModeActionGroup;        /** Interaction mode action group */
    ViewSettingsAction      _viewSettingsAction;                /** View settings action */
};
