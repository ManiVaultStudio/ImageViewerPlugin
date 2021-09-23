#include "ImageViewerWidget.h"

#include "util/PixelSelectionTool.h"

ImageViewerWidget::ImageViewerWidget(QWidget* parent) :
    QOpenGLWidget(parent),
    QOpenGLFunctions_3_3_Core(),
    _openGLInitialized(false),
    _pixelSelectionTool(this),
    _pixelSelectionToolRenderer(_pixelSelectionTool),
    _backgroundColor(100, 100, 100)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    setMouseTracking(true);

    
    this->installEventFilter(&_pixelSelectionTool);

    QObject::connect(&_pixelSelectionTool, &PixelSelectionTool::shapeChanged, [this]() {
        if (!isInitialized())
            return;

        _pixelSelectionToolRenderer.update();
        update();
    });
}

void ImageViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    qDebug() << "Initializing image viewer widget with context: " << context();

    std::string versionString = std::string((const char*)glGetString(GL_VERSION));

    qDebug() << versionString.c_str();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ImageViewerWidget::cleanup);

    _pixelSelectionToolRenderer.init();

    _openGLInitialized = true;
}

void ImageViewerWidget::resizeGL(int width, int height)
{
    _pixelSelectionToolRenderer.resize(QSize(width, height));
}

void ImageViewerWidget::paintGL()
{
    // Bind the frame buffer belonging to the widget
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // Clear the widget to the background color
    glClearColor(_backgroundColor.redF(), _backgroundColor.greenF(), _backgroundColor.blueF(), _backgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _pixelSelectionToolRenderer.render();
}

void ImageViewerWidget::cleanup()
{
    qDebug() << "Deleting image viewer widget, performing clean up...";
    
    _openGLInitialized = false;

    makeCurrent();

    _pixelSelectionToolRenderer.destroy();
}
