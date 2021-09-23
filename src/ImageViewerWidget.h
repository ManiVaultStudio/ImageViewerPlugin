#pragma once

#include "util/PixelSelectionTool.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>

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
     */
    ImageViewerWidget(QWidget* parent);

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
    bool                                    _openGLInitialized;         /** Whether OpenGL is initialized or not */
    PixelSelectionTool                      _pixelSelectionTool;        /** Pixel selection tool */
    std::unique_ptr<QOpenGLDebugLogger>     _openglDebugLogger;         /** OpenGL logger instance for debugging (only enabled in debug mode for performance reasons) */
    QRadialGradient                         _backgroundGradient;        /** Viewport gradient background */
    std::int32_t                            _keys;                      /** Currently pressed keyboard keys */
};
