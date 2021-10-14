#pragma once

#include "actions/IntegralAction.h"
#include "actions/StringAction.h"
#include "actions/GroupAction.h"

#include "util/DatasetRef.h"

#include "DataHierarchyItem.h"

#include "ImageData/Images.h"
#include "PointData.h"

#include <QDialog>

using namespace hdps::gui;
using namespace hdps::util;

class ImageViewerPlugin;

/**
 * Convert to images dataset dialog class
 *
 * @author Thomas Kroes
 */
class ConvertToImagesDatasetDialog : public QDialog
{
public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     * @param datasetName Name of the points dataset
     * @param parent Pointer to parent widget
     */
    ConvertToImagesDatasetDialog(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName);

    /** Destructor */
    ~ConvertToImagesDatasetDialog() = default;

    /** Get preferred size */
    QSize sizeHint() const override {
        return QSize(400, 200);
    }

    /** Get minimum size hint*/
    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    /** Get image size */
    QSize getImageSize() const;

    /**
     * Find the image size by walking up the tree and looking for images datasets
     * @param dataHierarchyItem Pointer to data hierarchy item
     */
    QSize findImageSize(hdps::DataHierarchyItem* dataHierarchyItem);

public: // Action getters

    IntegralAction& getImageWidthAction() { return _imageWidthAction; }
    IntegralAction& getImageHeightAction() { return _imageHeightAction; }
    IntegralAction& getNumberOfImagesAction() { return _numberOfImagesAction; }

protected:
    ImageViewerPlugin&          _imageViewerPlugin;         /** Reference to image viewer plugin */
    DatasetRef<hdps::DataSet>   _sourceDataset;             /** Reference to source dataset */
    DatasetRef<Images>          _imagesDataset;             /** Reference to images dataset */
    StringAction                _datasetNameAction;         /** Images dataset name action */
    IntegralAction              _imageWidthAction;          /** Image width action */
    IntegralAction              _imageHeightAction;         /** Image height action */
    IntegralAction              _numberOfImagesAction;      /** Number of images action */
    StringAction                _numberOfPixelsAction;      /** Number of pixels action */
    GroupAction                 _groupAction;               /** Group action */
};