#pragma once

#include "actions/GroupAction.h"

#include "SubsetAction.h"

#include <QDialog>

class ImageViewerPlugin;

/**
 * Create subset dialog class
 *
 * @author Thomas Kroes
 */
class CreateSubsetDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     * @param datasetName Name of the points dataset
     * @param parent Pointer to parent widget
     */
    CreateSubsetDialog(ImageViewerPlugin& imageViewerPlugin);

    /** Destructor */
    ~CreateSubsetDialog() = default;

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(250, 200);
    }

public: // Action getters

    ToggleAction& getFromRegionAction() { return _fromRegionAction; }
    StringAction& getNameAction() { return _nameAction; }

protected:
    ImageViewerPlugin&  _imageViewerPlugin;     /** Reference to image viewer plugin */
    ToggleAction        _fromRegionAction;      /** Whether to create an image set from the selected region of pixels */
    StringAction        _nameAction;            /** Subset name action */
    GroupAction         _groupAction;           /** Group action */
};