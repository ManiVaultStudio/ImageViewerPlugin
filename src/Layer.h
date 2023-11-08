#pragma once

#include "Renderable.h"

#include <actions/GroupsAction.h>

//#include "EditLayersAction.h"
#include "GeneralAction.h"
#include "ImageSettingsAction.h"
#include "SelectionAction.h"
#include "MiscellaneousAction.h"
#include "SubsetAction.h"

#include <util/Serializable.h>
#include <util/Interpolation.h>

#include <Set.h>
#include <ImageData/Images.h>

class ImageViewerPlugin;

class Layer : public mv::gui::GroupsAction, public Renderable
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

     /**
      * Construct with \p parent object and \p title
      * @param parent Pointer to parent object
      * @param title Title
      */
    Q_INVOKABLE Layer(QObject* parent, const QString& title);// );

    /** Destructor */
    virtual ~Layer();

    /**
     * Initialize the layer with an \p imageViewerPlugin and \p imagesDataset
     * @param imageViewerPlugin Pointer to image viewer plugin
     * @param imagesDataset Source images dataset
     */
    void initialize(ImageViewerPlugin* imageViewerPlugin, const mv::Dataset<Images>& imagesDataset);

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

    /** Get whether the layer is active or not */
    bool isActive() const;

    /** Invalidates the prop (triggers a re-render of all layers) */
    void invalidate();

    /**
     * Squeeze the layer into a rectangle whilst maintaining its aspect ratio
     * @param rectangle Rectangle to squeeze into
     */
    void scaleToFit(const QRectF& layersRectangle);

    /**
     * Paint
     * @param painter Reference to painter
     */
    void paint(QPainter& painter, const PaintFlag& paintFlags);

    /** Get source dataset */
    mv::Dataset<mv::DatasetImpl>& getSourceDataset() {
        return _sourceDataset;
    }

    /** Get images dataset */
    mv::Dataset<Images>& getImages() {
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
    void computeSelection(const QVector<QPoint>& mousePositions = QVector<QPoint>());

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

    /** Get the visible rectangle in world coordinates */
    QRectF getWorldBoundingRectangle() const override;

    /** Get the visible rectangle in screen coordinates */
    QRectF getScreenBoundingRectangle() const;

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

    /** Update image ROI */
    void updateRoi();

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: /** Action getters */

    GeneralAction& getGeneralAction() { return _generalAction; }
    ImageSettingsAction& getImageSettingsAction() { return _imageSettingsAction; }
    SelectionAction& getSelectionAction() { return _selectionAction; }
    MiscellaneousAction& getMiscellaneousAction() { return _miscellaneousAction; }
    SubsetAction& getSubsetAction() { return _subsetAction; }

signals:

    /**
     * Signals that the selection changed
     * @param selectedIndices Selected pixel indices
     */
    void selectionChanged(const std::vector<std::uint32_t>& selectedIndices);

protected:
    ImageViewerPlugin*                  _imageViewerPlugin;             /** Pointer to image viewer plugin */
    bool                                _active;                        /** Whether the layer is active (editable) */
    mv::Dataset<Images>               _imagesDataset;                 /** Smart pointer to images dataset */
    mv::Dataset<mv::DatasetImpl>    _sourceDataset;                 /** Smart pointer to source dataset of the images */
    std::vector<std::uint32_t>          _selectedIndices;               /** Indices of the selected pixels */
    GeneralAction                       _generalAction;                 /** General action */
    ImageSettingsAction                 _imageSettingsAction;           /** Image settings action */
    SelectionAction                     _selectionAction;               /** Selection action */
    MiscellaneousAction                 _miscellaneousAction;           /** Miscellaneous action */
    SubsetAction                        _subsetAction;                  /** Subset action */
    std::vector<std::uint8_t>           _selectionData;                 /** Selection data for selection prop */
    QRect                               _imageSelectionRectangle;       /** Selection boundaries in image coordinates */
    std::vector<std::uint8_t>           _maskData;                      /** Mask data for the image */

    friend class ImageViewerWidget;
    friend class ImageSettingsAction;
};
