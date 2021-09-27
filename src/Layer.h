#pragma once

#include "Renderable.h"
#include "LayerAction.h"

#include "util/DatasetRef.h"

#include "ImageData/Images.h"
#include "PointData.h"

using namespace hdps::util;

class ImageViewerPlugin;

class Layer : public Renderable
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     * @param datasetName Name of the images dataset
     */
    Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName);

    /** Get reference to image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin();

    /** Invalidates the prop (triggers a re-render of all layers) */
    void invalidate();

    void updateModelMatrix();
    LayerAction& getLayerAction() { return _layerAction; }

    DatasetRef<Images>& getImages() { return _images; }
    DatasetRef<Points>& getPoints() { return _points; }

    const QString getImagesDatasetName() const;

public: // Images wrapper functions

    const std::uint32_t getNumberOfImages() const;
    const QSize getImageSize() const;

public: // Points wrapper functions

    const std::uint32_t getNumberOfPoints() const;
    const std::uint32_t getNumberOfDimensions() const;
    const QStringList getDimensionNames() const;

public: // Selection

    /** Select all pixels in the image(s) */
    void selectAll();

    /** De-select all pixels in the image(s) */
    void selectNone();

    /** Invert the pixel selection in the image(s) */
    void invertSelection();

    /** Start the selection */
    void startSelection();

    /**
     * Compute selection
     * @param mousePositions Mouse positions
     */
    void computeSelection(const QVector<QPoint>& mousePositions);

    /** Publish selection */
    void publishSelection();

public: // View

    void zoomToExtents();

    /** Get the bounding rectangle */
    QRectF getWorldBoundingRectangle() const override;

protected:

    /**
     * Renders the props
     * @param parentMVP Parent model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

protected:
    ImageViewerPlugin&      _imageViewerPlugin;     /** Reference to image viewer plugin */
    DatasetRef<Images>      _images;                /** Reference to images dataset */
    DatasetRef<Points>      _points;                /** Reference to input points dataset of the images */
    LayerAction             _layerAction;           /** Layer settings action */

    friend class ImageViewerWidget;
};

using SharedLayer = QSharedPointer<Layer>;
