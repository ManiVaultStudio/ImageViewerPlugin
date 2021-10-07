#pragma once

#include "actions/IntegralAction.h"
#include "actions/StringAction.h"
#include "actions/GroupAction.h"

#include "util/DatasetRef.h"

#include "PointData.h"

#include <QDialog>

using namespace hdps::gui;
using namespace hdps::util;

/**
 * Points to images dialog class
 *
  * @author Thomas Kroes
 */
class PointsToImagesDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param datasetName Name of the points dataset
     * @param parent Pointer to parent widget
     */
    PointsToImagesDialog(const QString& datasetName, QWidget* parent = nullptr);

    /** Destructor */
    ~PointsToImagesDialog() = default;

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(350, 200);
    }

    /** Get image size */
    QSize getImageSize() const;

public: // Action getters

    IntegralAction& getImageWidthAction() {return _imageWidthAction; }
    IntegralAction& getImageHeightAction() {return _imageHeightAction; }
    IntegralAction& getNumberOfImagesAction() {return _numberOfImagesAction; }

protected:
    DatasetRef<Points>  _points;                    /** Reference to points */
    IntegralAction      _imageWidthAction;          /** Image width action */
    IntegralAction      _imageHeightAction;         /** Image height action */
    IntegralAction      _numberOfImagesAction;      /** Number of images action */
    StringAction        _numberOfPixelsAction;      /** Number of pixels action */
    StringAction        _notesAction;               /** Notes action */
    GroupAction         _groupAction;               /** Group action */
};