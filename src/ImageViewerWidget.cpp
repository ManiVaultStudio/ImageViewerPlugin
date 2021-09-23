#include "ImageViewerWidget.h"

#include "util/PixelSelectionTool.h"

#include <QKeyEvent>
#include <QPainter>
#include <QMessageBox>

ImageViewerWidget::ImageViewerWidget(QWidget* parent) :
    QOpenGLWidget(parent),
    QOpenGLFunctions_3_3_Core(),
    _openGLInitialized(false),
    _pixelSelectionTool(this),
    _pixelSelectionToolRenderer(_pixelSelectionTool),
    _backgroundColor(100, 100, 100),
    _openglDebugLogger(std::make_unique<QOpenGLDebugLogger>()),
    _backgroundGradient(),
    _keys()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //setFocusPolicy(Qt::StrongFocus);
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
        if (!isInitialized())
            return;

        //makeCurrent();
        _pixelSelectionToolRenderer.update();
        update();
    });
}

bool ImageViewerWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::KeyPress:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            if (!keyEvent->isAutoRepeat()) {
                if (keyEvent->key() == Qt::Key_Space) {
                    _keys |= Qt::Key_Space;
                    //_renderer->setInteractionMode(InteractionMode::Navigation);
                    setCursor(Qt::ClosedHandCursor);
                }
            }

            break;
        }

        case QEvent::KeyRelease:
        {
            auto keyEvent = static_cast<QKeyEvent*>(event);

            if (!keyEvent->isAutoRepeat()) {
                if (keyEvent->key() == Qt::Key_Space) {
                    _keys &= ~Qt::Key_Space;
                    //_renderer->setInteractionMode(InteractionMode::LayerEditing);
                    setCursor(Qt::ArrowCursor);
                }
            }

            break;
        }

        default:
            break;
    }

    //if (_keys & Qt::Key_Space)
    //    _renderer->handleEvent(event);
    //else
    //    _imageViewerPlugin->getLayersModel().dispatchEventToSelectedLayer(event);

    return QWidget::eventFilter(target, event);
}

void ImageViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ImageViewerWidget::cleanup);

    _pixelSelectionToolRenderer.init();

    _openGLInitialized = true;

#ifdef _DEBUG
    _openglDebugLogger->initialize();
#endif
}

void ImageViewerWidget::resizeGL(int width, int height)
{
    _pixelSelectionToolRenderer.resize(QSize(width, height));
}

void ImageViewerWidget::paintGL()
{
    try {
        // Bind the frame buffer belonging to the widget
        //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Reset the blending function
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        QPainter painter;

        painter.begin(this);

        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter.setPen(Qt::NoPen);
        painter.setBrush(_backgroundGradient);
        painter.drawRect(rect());
        painter.drawPixmap(rect(), _pixelSelectionTool.getAreaPixmap());
        painter.drawPixmap(rect(), _pixelSelectionTool.getShapePixmap());

        painter.beginNativePainting();

        // Draw the pixel selection tool on top of everything else
        //_pixelSelectionToolRenderer.render();

        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        /*
        auto root = layersModel.getLayer(QModelIndex());

        if (root)
            root->render(_renderer->getProjectionMatrix() * _renderer->getViewMatrix());
            */
        painter.endNativePainting();

        //layersModel.paint(&painter);

        

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

    _pixelSelectionToolRenderer.destroy();
}
