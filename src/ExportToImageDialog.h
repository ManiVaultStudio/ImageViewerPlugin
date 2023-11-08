#pragma once

#include <actions/DecimalAction.h>
#include <actions/StringAction.h>
#include <actions/GroupAction.h>

#include <QDialog>

using namespace mv::gui;

/**
 * Export to image dialog class
 *
 * Dialog class for exporting layers to image
 *
 * @author Thomas Kroes
 */
class ExportToImageDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ExportToImageDialog(QWidget* parent = nullptr);

    /** Destructor */
    ~ExportToImageDialog() = default;

    /** Get preferred size hint */
    QSize sizeHint() const override {
        return QSize(400, 100);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

public: // Action getters

    DecimalAction& getImageScaleFactorAction() {return _imageScaleFactorAction; }
    StringAction& getImageResolutionAction() {return _imageResolutionAction; }

protected:
    DecimalAction       _imageScaleFactorAction;    /** Image scale factor action */
    StringAction        _imageResolutionAction;     /** Image resolution action */
    GroupAction         _groupAction;               /** Group action */
};