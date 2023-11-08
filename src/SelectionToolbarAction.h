#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/ToggleAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/OptionAction.h>

class ImageViewerPlugin;
class ImageViewerWidget;

using namespace mv::gui;

/**
 * Selection toolbar action class
 *
 * Action class for selection toolbar
 *
 * @author Thomas Kroes
 */
class SelectionToolbarAction : public HorizontalToolbarAction
{
public:

    /** 
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    SelectionToolbarAction(ImageViewerPlugin& imageViewerPlugin);

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
    OptionAction& getModifierAction() { return _modifierAction; }

protected:
    ImageViewerPlugin&      _imageViewerPlugin;             /** Reference to image viewer plugin */
    ToggleAction            _rectangleSelectionAction;      /** Rectangle selection action */
    ToggleAction            _brushSelectionAction;          /** Brush selection action */
    ToggleAction            _lassoSelectionAction;          /** Lasso selection action */
    ToggleAction            _polygonSelectionAction;        /** Polygon selection action */
    ToggleAction            _sampleSelectionAction;         /** Sample selection action */
    ToggleAction            _roiSelectionAction;            /** ROI selection action */
    HorizontalGroupAction   _selectionAction;               /** Selection group action */
    OptionAction            _modifierAction;                /** Modifier action */
};
