#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>
#include <actions/HorizontalGroupAction.h>

#include "GlobalViewSettingsAction.h"

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

    ToggleAction& getRectangleSelectionAction() { return _rectangleSelectionAction; }
    ToggleAction& getBrushSelectionAction() { return _brushSelectionAction; }
    ToggleAction& getLassoSelectionAction() { return _lassoSelectionAction; }
    ToggleAction& getPolygonSelectionAction() { return _polygonSelectionAction; }
    ToggleAction& getSampleSelectionAction() { return _sampleSelectionAction; }
    ToggleAction& getRoiSelectionAction() { return _roiSelectionAction; }
    TriggerAction& getExportToImageAction() { return _exportToImageAction; }
    ViewSettingsAction& getGlobalViewSettingsAction() { return _viewSettingsAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;             /** Reference to image viewer plugin */
    ToggleAction            _rectangleSelectionAction;      /** Rectangle selection action */
    ToggleAction            _brushSelectionAction;          /** Brush selection action */
    ToggleAction            _lassoSelectionAction;          /** Lasso selection action */
    ToggleAction            _polygonSelectionAction;        /** Polygon selection action */
    ToggleAction            _sampleSelectionAction;         /** Sample selection action */
    ToggleAction            _roiSelectionAction;            /** ROI selection action */
    HorizontalGroupAction   _selectionAction;               /** Selection group action */
    TriggerAction           _exportToImageAction;           /** Export to image action */
    ViewSettingsAction      _viewSettingsAction;            /** View settings action */
};
