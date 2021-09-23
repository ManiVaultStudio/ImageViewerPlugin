#pragma once

#include "util/PixelSelectionTool.h"
#include "renderers/PixelSelectionToolRenderer.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

using namespace hdps::gui;

class PixelSelectionTool;

/**
 * 
 */
class ImageViewerWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
public: // Construction

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ImageViewerWidget(QWidget* parent);

    /** Get a reference to the pixel selection tool */
    PixelSelectionTool& getPixelSelectionTool() {
        return _pixelSelectionTool;
    }

protected: // OpenGL functions

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
    bool                            _openGLInitialized;             /** Whether OpenGL is initialized or not */
    PixelSelectionTool              _pixelSelectionTool;            /** Pixel selection tool */
    PixelSelectionToolRenderer      _pixelSelectionToolRenderer;    /** Pixel selection tool renderer */
    QColor                          _backgroundColor;               /** Viewer background color */
};
