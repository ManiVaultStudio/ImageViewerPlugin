#pragma once

#include "Renderable.h"
#include "LayersAction.h"
#include "GeneralAction.h"
#include "ImageAction.h"
#include "SelectionAction.h"
#include "SubsetAction.h"

#include "util/DatasetRef.h"

#include "ImageData/Images.h"
#include "PointData.h"

using namespace hdps::util;

class ImageViewerPlugin;

class Layer : public QObject, public Renderable
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     * @param datasetName Name of the images dataset
     */
    Layer(ImageViewerPlugin& imageViewerPlugin, const QString& datasetName);

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

    void updateModelMatrix();

    DatasetRef<Images>& getImages() { return _images; }
    DatasetRef<Points>& getPoints() { return _points; }

    const QString getImagesDatasetName() const;

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

public: // View

    // Zoom to layer extents
    void zoomToExtents();

    // Zoom to layer selection
    void zoomToSelection();

    /** Get the bounding rectangle */
    QRectF getWorldBoundingRectangle() const override;

protected:

    /**
     * Renders the props
     * @param modelViewProjectionMatrix Model view projection matrix
     */
    void render(const QMatrix4x4& modelViewProjectionMatrix) override;

    /** Update the view plugin window title when activated or when the layer name changes */
    void updateWindowTitle();

signals:

    /**
     * Signals that the selection changed
     * @param selectedIndices Selected pixel indices
     */
    void selectionChanged(const std::vector<std::uint32_t>& selectedIndices);

    /** Signals that the points data changed */
    void pointsDataChanged();

public: /** Action getters */

    LayersAction& getLayersAction();
    GeneralAction& getGeneralAction() { return _generalAction; }
    ImageAction& getImageAction() { return _imageAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }
    SubsetAction& getSubsetAction() { return _subsetAction; }

protected:
    ImageViewerPlugin&              _imageViewerPlugin;     /** Reference to image viewer plugin */
    bool                            _active;                /** Whether the layer is active (editable) */
    DatasetRef<Images>              _images;                /** Reference to images dataset */
    DatasetRef<Points>              _points;                /** Reference to input points dataset of the images */
    std::vector<std::uint32_t>      _selectedIndices;       /** Indices of the selected pixels */
    GeneralAction                   _generalAction;         /** General action */
    ImageAction                     _imageAction;           /** Image action */
    SelectionAction                 _selectionAction;       /** Selection action */
    SubsetAction                    _subsetAction;          /** Subset action */

    friend class ImageViewerWidget;
};
