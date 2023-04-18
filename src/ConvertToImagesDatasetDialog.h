#pragma once

#include <actions/IntegralAction.h>
#include <actions/StringAction.h>
#include <actions/GroupAction.h>
#include <actions/ToggleAction.h>
#include <DataHierarchyItem.h>
#include <ImageData/Images.h>
#include <PointData/PointData.h>
#include <Set.h>

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
     * @param dataset Dataset to convert
     * @param parent Pointer to parent widget
     */
    ConvertToImagesDatasetDialog(ImageViewerPlugin& imageViewerPlugin, hdps::Dataset<hdps::DatasetImpl>& dataset);

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

    /** Get smart pointer to the target images dataset */
    hdps::Dataset<Images> getTargetImagesDataset() const;

protected:

    /**
     * Find the image size by walking up the tree and looking for images datasets
     * @param dataHierarchyItem Reference to data hierarchy item
     */
    void findSourceImagesDataset(hdps::DataHierarchyItem& dataHierarchyItem);

public: // Action getters

    IntegralAction& getImageWidthAction() { return _imageWidthAction; }
    IntegralAction& getImageHeightAction() { return _imageHeightAction; }
    IntegralAction& getNumberOfImagesAction() { return _numberOfImagesAction; }

protected:
    ImageViewerPlugin&                  _imageViewerPlugin;             /** Reference to image viewer plugin */
    hdps::Dataset<hdps::DatasetImpl>    _sourceDataset;                 /** Reference to source dataset */
    hdps::Dataset<Images>               _sourceImagesDataset;           /** Reference to the source images dataset */
    hdps::Dataset<Images>               _targetImagesDataset;           /** Reference to the target images dataset */
    QRect                               _sourceRectangle;               /** Source rectangle (if a source images dataset is found) */
    StringAction                        _datasetNameAction;             /** Images dataset name action */
    IntegralAction                      _imageWidthAction;              /** Image width action */
    IntegralAction                      _imageHeightAction;             /** Image height action */
    IntegralAction                      _numberOfImagesAction;          /** Number of images action */
    StringAction                        _numberOfPixelsAction;          /** Number of pixels action */
    ToggleAction                        _useLinkedDataAction;           /** Use linked data action */
    GroupAction                         _groupAction;                   /** Group action */
};