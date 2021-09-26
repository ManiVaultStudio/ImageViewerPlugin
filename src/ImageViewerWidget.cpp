#include "ImageViewerWidget.h"
#include "LayersModel.h"
#include "Layer.h"

#include <QKeyEvent>
#include <QPainter>
#include <QMessageBox>

ImageViewerWidget::ImageViewerWidget(QWidget* parent, LayersModel& layersModel) :
    QOpenGLWidget(parent),
    QOpenGLFunctions_3_3_Core(),
    _layersModel(layersModel),
    _openGLInitialized(false),
    _pixelSelectionTool(this),
    _openglDebugLogger(std::make_unique<QOpenGLDebugLogger>()),
    _backgroundGradient(),
    _keys(),
    _renderer(this)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Configure pixel selection tool
    _pixelSelectionTool.setEnabled(false);
    _pixelSelectionTool.setMainColor(QColor(255, 156, 50));

    QSurfaceFormat surfaceFormat;

    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
    surfaceFormat.setSamples(4);

#ifdef __APPLE__
    // Ask for an OpenGL 3.3 Core Context as the default
    surfaceFormat.setVersion(3, 3);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //QSurfaceFormat::setDefaultFormat(defaultFormat);
#else
    // Ask for an OpenGL 4.3 Core Context as the default
    surfaceFormat.setVersion(4, 3);
    surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
    surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#endif

#ifdef _DEBUG
    surfaceFormat.setOption(QSurfaceFormat::DebugContext);
#endif

    surfaceFormat.setSamples(16);

    setFormat(surfaceFormat);

    const auto backgroundColor = QColor(50, 50, 50);

    _backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    _backgroundGradient.setCenter(0.5, 0.50);
    _backgroundGradient.setFocalPoint(0.5, 0.5);
    _backgroundGradient.setColorAt(0.0, backgroundColor);
    _backgroundGradient.setColorAt(0.7, backgroundColor);

    this->installEventFilter(this);

    QObject::connect(&_pixelSelectionTool, &PixelSelectionTool::shapeChanged, [this]() {
        if (isInitialized())
            update();
    });
}

bool ImageViewerWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::KeyPress:
        {
            // Get key that was pressed
            auto keyEvent = static_cast<QKeyEvent*>(event);

            // Do not handle repeating keys
            if (!keyEvent->isAutoRepeat()) {

                // Start navigating when the space key is pressed
                if (keyEvent->key() == Qt::Key_Space) {
                    _keys |= Qt::Key_Space;

                    // Set the render interaction mode to navigation
                    _renderer.setInteractionMode(Renderer::InteractionMode::Navigation);

                    // Provide a visual cue for navigation
                    setCursor(Qt::ClosedHandCursor);

                    // Disable the pixel selection tool, as we are navigating
                    _pixelSelectionTool.setEnabled(false);

                    // Re-render because the pixel selection tool pixmaps have changed
                    update();
                }
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            // Get key that was pressed
            auto keyEvent = static_cast<QKeyEvent*>(event);

            // Do not handle repeating keys
            if (!keyEvent->isAutoRepeat()) {

                // Stop navigating and start layer editing when the space key is released
                if (keyEvent->key() == Qt::Key_Space) {
                    _keys &= ~Qt::Key_Space;

                    // Set the render interaction mode to layer editing
                    _renderer.setInteractionMode(Renderer::InteractionMode::LayerEditing);

                    // Provide a visual cue for layer editing
                    setCursor(Qt::ArrowCursor);

                    // Enable the pixel selection tool again
                    _pixelSelectionTool.setEnabled(true);
                }
            }

            break;
        }

        default:
            break;
    }

    // Handle navigation in the renderer
    if (_keys & Qt::Key_Space)
        _renderer.handleEvent(event);

    return QWidget::eventFilter(target, event);
}

void ImageViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ImageViewerWidget::cleanup);

    _openGLInitialized = true;

#ifdef _DEBUG
    _openglDebugLogger->initialize();
#endif

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
}

void ImageViewerWidget::resizeGL(int width, int height)
{
}

void ImageViewerWidget::paintGL()
{
    try {
        QPainter painter;

        // Begin mixed OpenGL/native painting
        painter.begin(this);
        {
            // Draw the background
            painter.setBrush(_backgroundGradient);
            painter.drawRect(rect());

            // Draw layers with OpenGL
            painter.beginNativePainting();
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                auto layersSorted = _layersModel.getLayers();

                // Sort the layers
                std::reverse(layersSorted.begin(), layersSorted.end());

                // Draw the image layers
                for (auto& layer : layersSorted)
                    layer->render(_renderer.getProjectionMatrix() * _renderer.getViewMatrix());
            }
            painter.endNativePainting();

            // Draw the pixel selection tool overlays
            painter.drawPixmap(rect(), _pixelSelectionTool.getAreaPixmap());
            painter.drawPixmap(rect(), _pixelSelectionTool.getShapePixmap());
        }
        painter.end();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Rendering failed", e.what());
    }
    catch (...) {
        QMessageBox::critical(nullptr, "Rendering failed", "An unhandled exception occurred");
    }

#ifdef _DEBUG
    for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
        switch (message.severity())
        {
            case QOpenGLDebugMessage::HighSeverity:
                qDebug() << message;
                break;

            default:
                break;
        }
        
#endif
}

void ImageViewerWidget::cleanup()
{
    qDebug() << "Deleting image viewer widget, performing clean up...";
    
    _openGLInitialized = false;

    makeCurrent();
}
