#pragma once

#include "Renderable.h"
#include "LayersAction.h"
#include "GeneralAction.h"
#include "ImageAction.h"
#include "SelectionAction.h"

#include "util/Interpolation.h"
#include "event/EventListener.h"

#include "Set.h"
#include "ImageData/Images.h"

using namespace hdps::util;

class ImageViewerPlugin;

class Layer : public QObject, public Renderable, public hdps::EventListener
{
    Q_OBJECT

    /** Paint flags */
    enum PaintFlag {
        Bounds              = 0x001,
        Label               = 0x002,
        SelectionRectangle  = 0x004,
        Sample              = 0x008
    };

public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     * @param dataset Smart pointer to images dataset
     */
    Layer(ImageViewerPlugin& imageViewerPlugin, const hdps::Dataset<Images>& imagesDataset);

    /** Destructor */
    virtual ~Layer();

    /** Get reference to image viewer plugin */
    ImageViewerPlugin& getImageViewerPlugin();

    /**
     * Get the context menu
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr);

    /** Activate the layer */
    void activate();

    /** De-activate the layer */
    void deactivate();

    /** Invalidates the prop (triggers a re-render of all layers) */
    void invalidate();

    /**
     * Squeeze the layer into a rectangle whilst maintaining its aspect ratio
     * @param rectangle Rectangle to squeeze into
     */
    void scaleToFit(const QRectF& rectangle);

    /**
     * Paint
     * @param painter Reference to painter
     */
    void paint(QPainter& painter, const PaintFlag& paintFlags);

    /** Get source dataset */
    hdps::Dataset<hdps::DatasetImpl> getSourceDataset() {
        return _sourceDataset;
    }

    /** Get source dataset of a specific dataset type */
    template<typename DatasetType>
    hdps::Dataset<DatasetType> getSourceDataset() {
        return _sourceDataset;
    }

    /** Get const source dataset of a specific dataset type */
    template<typename DatasetType>
    hdps::Dataset<DatasetType> getSourceDataset() const {
        return const_cast<Layer*>(this)->getSourceDataset<DatasetType>();
    }

    /** Get images dataset */
    hdps::Dataset<Images> getImages() {
        return _imagesDataset;
    }

    /** Get the globally unique identifier of the images dataset */
    const QString getImagesDatasetId() const;

public: // Images wrapper functions

    const std::uint32_t getNumberOfImages() const;
    const QSize getImageSize() const;

public: // Points wrapper functions

    const QStringList getDimensionNames() const;

public: // Selection

    /** Select all pixels in the image(s) */
    void selectAll();

    /** De-select all pixels in the image(s) */
    void selectNone();

    /** Invert the pixel selection in the image(s) */
    void selectInvert();

    /** Start the selection */
    void startSelection();

    /**
     * Reset off-screen selection buffer
     * @param mousePositions Mouse positions
     */
    void resetSelectionBuffer();

    /**
     * Compute selection
     * @param mousePositions Mouse positions
     */
    void computeSelection(const QVector<QPoint>& mousePositions);

    /** Publish selection */
    void publishSelection();

    /** Compute the selected indices */
    void computeSelectionIndices();

    /** Get indices of the selected pixels */
    std::vector<std::uint32_t>& getSelectedIndices();

    /** Get indices of the selected pixel indices */
    const std::vector<std::uint32_t>& getSelectedIndices() const;

    /** Get selection rectangle in image coordinates */
    QRect getImageSelectionRectangle() const;

    /** Get selection rectangle in world coordinates */
    QRectF getWorldSelectionRectangle() const;

public: // View

    // Zoom to layer extents
    void zoomToExtents();

    // Zoom to layer selection
    void zoomToSelection();

    /** Get the bounding rectangle */
    QRectF getWorldBoundingRectangle() const override;

protected: // Rendering

    /**
     * Renders the props
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

    /** Update the model transformation matrix (used in OpenGL) */
    void updateModelMatrix();

    /**
     * Assign the color map image to the image rendering prop
     * @param colorMapImage Color map image
     * @param interpolationType Interpolation type
     */
    void setColorMapImage(const QImage& colorMapImage, const InterpolationType& interpolationType);

protected: // Miscellaneous

    /** Update the view plugin window title when activated or when the layer name changes */
    void updateWindowTitle();

signals:

    /**
     * Signals that the selection changed
     * @param selectedIndices Selected pixel indices
     */
    void selectionChanged(const std::vector<std::uint32_t>& selectedIndices);

public: /** Action getters */

    LayersAction& getLayersAction();
    GeneralAction& getGeneralAction() { return _generalAction; }
    ImageAction& getImageAction() { return _imageAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }

protected:
    ImageViewerPlugin&                  _imageViewerPlugin;             /** Reference to image viewer plugin */
    bool                                _active;                        /** Whether the layer is active (editable) */
    hdps::Dataset<Images>               _imagesDataset;                 /** Smart pointer to images dataset */
    hdps::Dataset<hdps::DatasetImpl>    _sourceDataset;                 /** Smart pointer to source dataset of the images */
    std::vector<std::uint32_t>          _selectedIndices;               /** Indices of the selected pixels */
    GeneralAction                       _generalAction;                 /** General action */
    ImageAction                         _imageAction;                   /** Image action */
    SelectionAction                     _selectionAction;               /** Selection action */
    std::vector<std::uint8_t>           _selectionData;                 /** Selection data for selection prop */
    QRect                               _imageSelectionRectangle;       /** Selection boundaries in image coordinates */
    QVector<float>                      _colorData;                     /** Color data for the specified dimension */

    friend class ImageViewerWidget;
    friend class ImageAction;
};
