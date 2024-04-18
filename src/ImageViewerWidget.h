#pragma once

#include "LayersRenderer.h"

#include <util/PixelSelectionTool.h>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>

class ImageViewerPlugin;
class Layer;

using namespace mv::util;

/**
 * Image viewer widget class
 *
 * Mixed OpenGL/native rendering class for displaying image layers
 * 
 * @author Thomas Kroes
 */
class ImageViewerWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
Q_OBJECT

public:

    /**
     * Interaction mode
     * Defines interaction modes that are possible in the image viewer widget
    */
    enum InteractionMode
    {
        None,               /** No interaction takes place */
        Navigation,         /** The image view position and zoom are manipulated */
        Selection           /** Layer editing interaction */
    };

    /** Maps interaction mode enum to name */
    static const QMap<InteractionMode, QString> interactionModes;

public:

    /**
     * Constructor
     * @param imageViewerPlugin Reference to image viewer plugin
     */
    ImageViewerWidget(ImageViewerPlugin& imageViewerPlugin);

    /**
     * Widget event capture
     *@param target Target object
     *@param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Get a reference to the pixel selection tool */
    PixelSelectionTool& getPixelSelectionTool() {
        return _pixelSelectionTool;
    }

    /** Get viewport background color */
    QColor getBackgroundColor() const {
        return _backgroundColor;
    }

    /**
     * Set viewport background color
     * @param backgroundColor The background color
     */
    void setBackgroundColor(const QColor& backgroundColor) {
        _backgroundColor = backgroundColor;
        update();
    }

    /** Get a reference to the pixel selection tool */
    LayersRenderer& getRenderer() {
        return _renderer;
    }

    /** Get mouse positions */
    QVector<QPoint> getMousePositions() { return _mousePositions; }

    /** Export layers to image */
    void exportToImage();

protected: // OpenGL

    /** Initializes the OpenGL window */
    void initializeGL() override;

    /**
     * Invoked when the view resizes
     * @param width New width of the view
     * @param height New height of the view
     */
    void resizeGL(int width, int height) override;

    /** The actual OpenGL painting takes place here */
    void paintGL() override;

    /** Perform cleanup when viewer widget is destroyed */
    void cleanup();

    /** Get a pointer to the layer that is beneath the cursor */
    Layer* getLayerBeneathCursor();

public: // Miscellaneous

    /** Returns the current interaction mode */
    InteractionMode getInteractionMode() const;

    /**
     * Sets the current interaction mode
     * @param interactionMode The interaction mode
     */
    void setInteractionMode(const InteractionMode& interactionMode);

    /**
     * Get bounding rectangle of the world
     * @param visibleOnly Only include visible layers
     */
    QRectF getWorldBoundingRectangle(bool visibleOnly = true) const;

    /** Update bounding rectangle */
    void updateWorldBoundingRectangle();

signals:

    /** Signals that pixel selection process started */
    void pixelSelectionStarted();

    /**
     * Signals that the mouse positions changed
     * @param mousePositions Mouse positions
     */
    void mousePositionsChanged(const QVector<QPoint>& mousePositions);

    /** Signals that pixel selection process ended */
    void pixelSelectionEnded();

    /**
     * Signals that the interaction mode changed
     * @param interactionMode Interaction mode
     */
    void interactionModeChanged(const InteractionMode& interactionMode);

    /** Signals that the viewport navigation started */
    void navigationStarted();

    /** Signals that the viewport has changed */
    void viewportChanged();

    /** Signals that the viewport navigation ended */
    void navigationEnded();

protected:
    ImageViewerPlugin&                      _imageViewerPlugin;         /** Reference to image viewer plugin */
    bool                                    _openGLInitialized;         /** Whether OpenGL is initialized or not */
    PixelSelectionTool                      _pixelSelectionTool;        /** Pixel selection tool */
    std::unique_ptr<QOpenGLDebugLogger>     _openglDebugLogger;         /** OpenGL logger instance for debugging (only enabled in debug mode for performance reasons) */
    QColor                                  _backgroundColor;           /** Viewport background color */
    std::int32_t                            _keys;                      /** Currently pressed keyboard keys */
    QVector<QPoint>                         _mousePositions;            /** Recorded mouse positions */
    int                                     _mouseButtons;              /** State of the left, middle and right mouse buttons */
    LayersRenderer                          _renderer;                  /** Layers OpenGL renderer */
    InteractionMode                         _interactionMode;           /** Interaction mode e.g. navigation and layer editing */
};
