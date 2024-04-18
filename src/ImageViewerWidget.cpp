#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"
#include "LayersModel.h"
#include "Layer.h"
#include "ImageProp.h"
#include "ExportToImageDialog.h"

#include <util/Exception.h>

#include <ClusterData/ClusterData.h>

#include <QKeyEvent>
#include <QPainter>
#include <QOpenGLFramebufferObject>
#include <QOffscreenSurface>

#include <chrono>
#include <thread>

const QMap<ImageViewerWidget::InteractionMode, QString> ImageViewerWidget::interactionModes = {
    { ImageViewerWidget::None, "No interaction" },
    { ImageViewerWidget::Navigation, "Navigation" },
    { ImageViewerWidget::Selection, "Selection" }
};

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin& imageViewerPlugin) :
    QOpenGLWidget(&imageViewerPlugin.getWidget()),
    QOpenGLFunctions_3_3_Core(),
    _imageViewerPlugin(imageViewerPlugin),
    _openGLInitialized(false),
    _pixelSelectionTool(this),
    _openglDebugLogger(std::make_unique<QOpenGLDebugLogger>()),
    _backgroundColor(Qt::darkGray),
    _keys(),
    _mousePositions(),
    _mouseButtons(),
    _renderer(this),
    _interactionMode(InteractionMode::Selection)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Configure pixel selection tool
    _pixelSelectionTool.setEnabled(false);
    _pixelSelectionTool.setMainColor(QColor(255, 156, 50));

    QSurfaceFormat surfaceFormat;

    surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);

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

    this->installEventFilter(this);

    QObject::connect(&_pixelSelectionTool, &PixelSelectionTool::shapeChanged, [this]() {
        if (isInitialized())
            update();
    });
}

bool ImageViewerWidget::eventFilter(QObject* target, QEvent* event)
{
    // Get number of mouse positions
    const auto numberOfMousePositions = _mousePositions.size();

    // Notify listeners that unique mouse positions changed
    const auto notifyMousePositionsChanged = [this]() {

        // Only notify in layer editing mode
        if (_interactionMode != Selection)
            return;

        QList<QPoint> uniqueMousePositions;

        // Get unique mouse positions
        for (auto mousePosition : _mousePositions) {
            if (!uniqueMousePositions.contains(mousePosition))
                uniqueMousePositions.append(mousePosition);
        }
        
        // Notify listeners that the mouse positions changed
        if (!uniqueMousePositions.isEmpty())
            emit mousePositionsChanged(uniqueMousePositions.toVector());
    };

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
                    setInteractionMode(InteractionMode::Navigation);

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
                    setInteractionMode(InteractionMode::Selection);

                    // Reset mouse positions
                    _mousePositions.clear();

                    // Enable the pixel selection tool again
                    _pixelSelectionTool.setEnabled(true);
                }
            }

            break;
        }

        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            switch (_interactionMode)
            {
                case Navigation:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton)
                        _mousePositions << mouseEvent->pos();

                    setCursor(Qt::ClosedHandCursor);

                    _pixelSelectionTool.setEnabled(false);

                    // Notify others that navigation has started
                    emit navigationStarted();

                    break;
                }

                case Selection:
                {
                    _pixelSelectionTool.setEnabled(true);

                    switch (_pixelSelectionTool.getType())
                    {
                        case PixelSelectionType::Rectangle:
                        case PixelSelectionType::Brush:
                        case PixelSelectionType::Lasso:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions = { mouseEvent->pos() };
                                emit pixelSelectionStarted();
                            }

                            break;
                        }

                        case PixelSelectionType::Polygon:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions << mouseEvent->pos();

                                if (_mousePositions.count() == 1) {
                                    _mousePositions << mouseEvent->pos();

                                    emit pixelSelectionStarted();
                                }
                            }

                            if (mouseEvent->buttons() & Qt::RightButton) {
                                _mousePositions.clear();

                                emit pixelSelectionEnded();
                            }

                            break;
                        }

                        case PixelSelectionType::Sample:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions = { mouseEvent->pos() };
                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        case PixelSelectionType::ROI:
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            notifyMousePositionsChanged();

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            switch (_interactionMode)
            {
                case Navigation:
                {
                    setCursor(Qt::OpenHandCursor);

                    // Notify others that navigation has ended
                    emit navigationEnded();

                    break;
                }

                case Selection:
                {
                    auto mouseEvent = static_cast<QMouseEvent*>(event);

                    switch (_pixelSelectionTool.getType())
                    {
                        case PixelSelectionType::Rectangle:
                        case PixelSelectionType::Brush:
                        case PixelSelectionType::Lasso:
                        {
                            if (mouseEvent->button() == Qt::LeftButton) {
                                _mousePositions.clear();
                                emit pixelSelectionEnded();
                            }

                            break;
                        }

                        case PixelSelectionType::Polygon:
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            switch (_interactionMode)
            {
                case Navigation:
                {
                    _mousePositions << mouseEvent->pos();

                    if (mouseEvent->buttons() & Qt::LeftButton && numberOfMousePositions >= 2) {

                        // Compute the translation between the two last mouse positions and compute the (zoom level corrected) delta
                        const auto previousMousePosition    = _mousePositions[numberOfMousePositions - 2];
                        const auto currentMousePosition     = _mousePositions[numberOfMousePositions - 1];
                        const auto panVector                = currentMousePosition - previousMousePosition;

                        // Pan the view and render
                        _renderer.panBy(QPoint(-panVector.x(), -panVector.y()));
                        _renderer.render();
                    }

                    // Notify others that the viewport changed
                    emit viewportChanged();

                    break;
                }

                case Selection:
                {
                    switch (_pixelSelectionTool.getType())
                    {
                        case PixelSelectionType::Rectangle:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                if (_mousePositions.count() < 2)
                                    _mousePositions << mouseEvent->pos();
                                else
                                    _mousePositions.last() = mouseEvent->pos();

                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        case PixelSelectionType::Brush:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions << mouseEvent->pos();
                                
                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        case PixelSelectionType::Lasso:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions << mouseEvent->pos();

                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        case PixelSelectionType::Polygon:
                        {
                            if (numberOfMousePositions > 1) {
                                _mousePositions.last() = mouseEvent->pos();

                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        case PixelSelectionType::Sample:
                        {
                            if (mouseEvent->buttons() & Qt::LeftButton) {
                                _mousePositions = { mouseEvent->pos() };
                                notifyMousePositionsChanged();
                            }

                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::Wheel:
        {
            auto wheelEvent = static_cast<QWheelEvent*>(event);

            const auto zoomCenter = wheelEvent->position().toPoint();


            switch (_interactionMode)
            {
                case Navigation:
                {
                    if (wheelEvent->angleDelta().ry() < 0) {
                        _renderer.zoomAround(zoomCenter, 1.0f - _renderer.getZoomSensitivity());
                    }
                    else {
                        _renderer.zoomAround(zoomCenter, 1.0f + _renderer.getZoomSensitivity());
                    }

                    // Notify others that navigation has started, the viewport changed and navigation ended
                    emit navigationStarted();
                    emit viewportChanged();
                    emit navigationEnded();

                    break;
                }

                case None:
                case Selection:
                    break;
            }

            notifyMousePositionsChanged();

            _renderer.render();

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void ImageViewerWidget::exportToImage()
{
    try {

        qDebug() << "Export layers to image";

        // Create export to image dialog
        ExportToImageDialog exportToImageDialog(this);

        // Show the dialog and catch the result
        const auto result = exportToImageDialog.exec();

        if (result == 1) {
            makeCurrent();
            {
                initializeGL();

                // Get the world bounding box
                const auto worldBoundingBox = _renderer.getWorldBoundingBox();

                // Establish the off-screen FBO size
                const auto fboSize = exportToImageDialog.getImageScaleFactorAction().getValue() * worldBoundingBox.size();

                // Create off-screen FBO
                QScopedPointer<QOpenGLFramebufferObject> fbo(new QOpenGLFramebufferObject(size(), QOpenGLFramebufferObject::NoAttachment, GL_TEXTURE_2D));

                //glBindFramebuffer(GL_FRAMEBUFFER, );
                if (!fbo->bind())
                    throw std::runtime_error("Unable to bind FBO");

                if (!fbo->isValid())
                    throw std::runtime_error("FBO is not valid");

                
                paintGL();
                /*
                initializeOpenGLFunctions();

                glViewport(0, 0, fbo->width(), fbo->height());

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                //glEnable(GL_DEPTH_TEST);
                //glClearColor(0.1, 0.1, 0.1, 0.5);
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                //glBindTexture(GL_TEXTURE_2D, fbo->texture());
                auto layersSorted = _layersModel.getLayers();

                // Sort the layers
                std::reverse(layersSorted.begin(), layersSorted.end());

                // Draw the image layers
                for (auto& layer : layersSorted)
                    layer->render(_renderer.getProjectionMatrix() * _renderer.getViewMatrix());
                    */

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                fbo->toImage().save("export.jpg");

                // Release the FBO
                fbo->release();

            }
            doneCurrent();

            /*
            // Compute half of the FBO size
            const auto halfSize = QSizeF(fbo->size()) / 2;

            QMatrix4x4 projectionMatrix;

            // Compute the orthogonal projection matrix
            projectionMatrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

            QMatrix4x4 viewMatrix;

            // Construct look-at parameters
            const auto eye = QVector3D(worldBoundingBox.center().x(), worldBoundingBox.center().y(), 1);
            const auto center = QVector3D(worldBoundingBox.center().x(), worldBoundingBox.center().y(), 0);
            const auto up = QVector3D(0, 1, 0);

            // Compute view matrix
            viewMatrix.lookAt(eye, center, up);
            */
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to export layer(s) to image: %1", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to export layer(s) to image: %1");
    }
}

void ImageViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ImageViewerWidget::cleanup);

    _openGLInitialized = true;

#ifdef _DEBUG
    _openglDebugLogger->initialize();
#endif

    glEnable(GL_DEPTH_TEST);
}

void ImageViewerWidget::resizeGL(int width, int height)
{
    // Update bounds
    updateWorldBoundingRectangle();
}

void ImageViewerWidget::paintGL()
{
    try {
        QPainter painter;

        // Begin mixed OpenGL/native painting
        if (!painter.begin(this))
            throw std::runtime_error("Unable to begin painting");

        // Enable anti-aliasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        // Draw the background
        painter.setBrush(_backgroundColor);
        painter.drawRect(rect());

        auto layersSorted = _imageViewerPlugin.getLayersModel().getLayers();

        // Sort the layers
        std::reverse(layersSorted.begin(), layersSorted.end());

        // Draw the image layers
        for (auto& layer : layersSorted) {
                
            // Draw layer with OpenGL
            painter.beginNativePainting();
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                layer->render(_renderer.getProjectionMatrix() * _renderer.getViewMatrix());
            }
            painter.endNativePainting();

            // Draw selection rectangle with native rendering
            layer->paint(painter, Layer::SelectionRectangle);
        }

        Layer* activeLayer = nullptr;
        
        // Draw the layer label and layer bounds with native rendering
        for (auto layer : layersSorted) {
            layer->paint(painter, Layer::Bounds);
            layer->paint(painter, Layer::Label);

            if (layer->isActive())
                activeLayer = layer;
        }

        // Draw the pixel selection tool overlays if the pixel selection tool is enabled
        if (_pixelSelectionTool.isEnabled()) {

            // Draw the area and shape pixmaps in the viewport
            painter.drawPixmap(rect(), _pixelSelectionTool.getAreaPixmap());
            painter.drawPixmap(rect(), _pixelSelectionTool.getShapePixmap());
        }

        if (activeLayer && static_cast<PixelSelectionType>(activeLayer->getSelectionAction().getPixelSelectionAction().getTypeAction().getCurrentIndex()) == PixelSelectionType::ROI) {

            // Prevent infinite updates
            QSignalBlocker pixelSelectionToolBlocker(&_pixelSelectionTool);

            // Force paint in selection tool because it does not rely on mouse input
            _pixelSelectionTool.update();

            // Draw the area and shape pixmaps in the viewport
            painter.drawPixmap(rect(), _pixelSelectionTool.getAreaPixmap());
            painter.drawPixmap(rect(), _pixelSelectionTool.getShapePixmap());
        }

        // Show cluster name when in sample selection mode
        if (_interactionMode == Selection) {

            // Only deal with sample selection type
            if (_pixelSelectionTool.getType() != PixelSelectionType::Sample)
                return;

            // Get layer beneath cursor (if any)
            auto layer = getLayerBeneathCursor();
            
            // Draw sample info if a layer is found
            if (layer)
                layer->paint(painter, Layer::PaintFlag::Sample);
        }

        // End mixed OpenGL/native painting
        painter.end();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Rendering failed", e);
    }
    catch (...) {
        exceptionMessageBox("Rendering failed");
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

Layer* ImageViewerWidget::getLayerBeneathCursor()
{
    // Iterate over all active layers from front to back
    for (auto layer : _imageViewerPlugin.getLayersModel().getLayers()) {

        // Only do an intersection test when the layer is visible
        if (!layer->getGeneralAction().getVisibleAction().isChecked())
            continue;

        // Get mouse position in widget coordinates
        const auto mousePositionWidget = QWidget::mapFromGlobal(QCursor::pos());

        // Get mouse position in world coordinates
        const auto mousePositionWorld = _renderer.getScreenPointToWorldPosition(layer->getModelViewMatrix() * layer->getPropByName<ImageProp>("ImageProp")->getModelMatrix(), mousePositionWidget);

        // Establish whether the mouse position is contained by its bounding box and return the layer if so
        if (layer->getImagesDataset()->getVisibleRectangle().contains(mousePositionWorld.toPoint()))
            return layer;
    }

    return nullptr;
}

ImageViewerWidget::InteractionMode ImageViewerWidget::getInteractionMode() const
{
    return _interactionMode;
}

void ImageViewerWidget::setInteractionMode(const InteractionMode& interactionMode)
{
    if (interactionMode == _interactionMode)
        return;

    qDebug() << "Set interaction mode to" << interactionModes.value(interactionMode);

    _interactionMode = interactionMode;

    // Enable/disable the pixel selection tool depending on the interaction mode
    //_pixelSelectionTool.setEnabled(_interactionMode == Selection);

    // Provide a visual cursor cue
    setCursor(_interactionMode == Selection ? Qt::ArrowCursor : Qt::OpenHandCursor);

    // Render
    update();

    // Notify others that the interaction mode changed
    emit interactionModeChanged(_interactionMode);
}

QRectF ImageViewerWidget::getWorldBoundingRectangle(bool visibleOnly /*= true*/) const
{
    QRectF worldBoundingRectangle;

    for (const auto& layer : _imageViewerPlugin.getLayersModel().getLayers()) {
        if (visibleOnly && !layer->getGeneralAction().getVisibleAction().isChecked())
            continue;
        
        worldBoundingRectangle |= layer->getWorldBoundingRectangle();
    }

    return worldBoundingRectangle;
}

void ImageViewerWidget::updateWorldBoundingRectangle()
{
    const auto worldBoundingRectangle = getWorldBoundingRectangle();

    if (!worldBoundingRectangle.isValid())
        return;

    getRenderer().setWorldBoundingRectangle(worldBoundingRectangle);
}
