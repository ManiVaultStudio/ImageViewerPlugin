#pragma once

#include "Renderer.h"

#include "util/PixelSelectionTool.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>

class LayersModel;

/**
 * Image viewer widget class
 *
 * Mixed OpenGL/native rendering class for displaying image layers
 * 
 * @author Thomas Kroes
 */
class ImageViewerWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
public: // Construction

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param layersModel Reference to layers model
     */
    ImageViewerWidget(QWidget* parent, LayersModel& layersModel);

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

    /** Get a reference to the pixel selection tool */
    Renderer& getRenderer() {
        return _renderer;
    }

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

protected:
    LayersModel&                            _layersModel;               /** Reference to layers model */
    bool                                    _openGLInitialized;         /** Whether OpenGL is initialized or not */
    PixelSelectionTool                      _pixelSelectionTool;        /** Pixel selection tool */
    std::unique_ptr<QOpenGLDebugLogger>     _openglDebugLogger;         /** OpenGL logger instance for debugging (only enabled in debug mode for performance reasons) */
    QRadialGradient                         _backgroundGradient;        /** Viewport gradient background */
    std::int32_t                            _keys;                      /** Currently pressed keyboard keys */
    Renderer                                _renderer;                  /** Layers OpenGL renderer */
};
