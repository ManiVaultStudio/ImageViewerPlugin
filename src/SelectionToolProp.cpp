#include "SelectionToolProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "LayersRenderer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"

#include "util/FileUtil.h"
#include "util/Exception.h"
#include "util/PixelSelection.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept>

using namespace hdps::util;

SelectionToolProp::SelectionToolProp(Layer& layer, const QString& name) :
    Prop(layer, name),
    _layer(layer),
    _fbo()
{
    addShape<QuadShape>("Quad");

    addShaderProgram("SelectionTool");
    addShaderProgram("SelectionToolOffScreen");
    initialize();
}

void SelectionToolProp::initialize()
{
    try
    {
        Prop::initialize();

        getRenderer().bindOpenGLContext();
        {
            // Load shader programs
            loadSelectionToolShaderProgram();
            loadSelectionToolOffScreenShaderProgram();

            _initialized = true;
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection tool prop initialization failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection tool prop initialization failed");
    }
}

void SelectionToolProp::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        if (!canRender())
            return;

        const auto shape                        = getShapeByName<QuadShape>("Quad");
        const auto selectionToolShaderProgram   = getShaderProgramByName("SelectionTool");

        // Bind shader program
        if (_fbo.isNull())
            throw std::runtime_error("FBO not initialized");

        // Bind shader program
        if (!selectionToolShaderProgram->bind())
            throw std::runtime_error("Unable to bind quad shader program");

        glBindTexture(GL_TEXTURE_2D, _fbo->texture());

        // Get reference to selection action
        auto& selectionAction = _layer.getSelectionAction();

        // Configure shader program
        selectionToolShaderProgram->setUniformValue("offScreenTexture", 0);
        selectionToolShaderProgram->setUniformValue("color", QColor(255, 156, 50, 100));
        selectionToolShaderProgram->setUniformValue("opacity", selectionAction.getOverlayOpacity().getValue());
        selectionToolShaderProgram->setUniformValue("transform", modelViewProjectionMatrix * _renderable.getModelMatrix() * getModelMatrix());

        // Render the quad
        shape->render();

        // Release the shader program
        selectionToolShaderProgram->release();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection tool prop rendering failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection tool prop rendering failed");
    }
}

QRectF SelectionToolProp::getWorldBoundingRectangle() const
{
    return getShapeByName<QuadShape>("Quad")->getRectangle();
}

void SelectionToolProp::setGeometry(const QRect& imageRectangle)
{
    try {
        qDebug() << "Set selection tool prop geometry:" << imageRectangle;

        getRenderer().bindOpenGLContext();
        {
            // Assign the rectangle to the quad shape
            getShapeByName<QuadShape>("Quad")->setRectangle(imageRectangle);

            // Update the model matrix
            QMatrix4x4 modelMatrix;

            // Establish center
            const auto center = imageRectangle.center();

            // Compute the model matrix
            modelMatrix.translate(-center.x() + imageRectangle.left(), -center.y() + imageRectangle.top(), 0.0f);

            // Assign model matrix
            setModelMatrix(modelMatrix);

            // Create FBO when none exists
            if (_fbo.isNull())
                _fbo.reset(new QOpenGLFramebufferObject(imageRectangle.width(), imageRectangle.height()));
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection prop set geometry failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop set geometry failed");
    }
}

void SelectionToolProp::compute(const QVector<QPoint>& mousePositions)
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Check if FBO is created
            if (_fbo.isNull())
                throw std::runtime_error("FBO not created");

            // Check if FBO is valid
            if (!_fbo->isValid())
                throw std::runtime_error("FBO not valid");

            // Bind FBO for off-screen rendering
            if (!_fbo->bind())
                throw std::runtime_error("Unable to bind FBO");

            // Get quad shape and compute the model-view-matrix
            const auto quadShape        = getShapeByName<QuadShape>("Quad");
            const auto quadRectangle    = quadShape->getRectangle();
            const auto modelViewMatrix  = _layer.getRenderer().getViewMatrix() * _renderable.getModelMatrix() * getModelMatrix();

            // Create viewport with the same size as the FBO a
            glViewport(0.0f, 0.0f, _fbo->width(), _fbo->height());

            QMatrix4x4 transform;

            // Create orthogonal transformation matrix
            transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

            // Get reference to selection action
            auto& selectionAction = _layer.getSelectionAction();

            // Get shader program for the off-screen rendering
            const auto selectionToolOffScreenShaderProgram = getShaderProgramByName("SelectionToolOffScreen");

            // Bind the quad vertex array object buffer
            quadShape->getVAO().bind();

            // Bind shader program
            if (!selectionToolOffScreenShaderProgram->bind())
                throw std::runtime_error("Unable to bind off screen shader program");

            // Bind the FBO texture for off-screen rendering
            glBindTexture(GL_TEXTURE_2D, _fbo->texture());

            // Get selection type and brush radius
            const auto selectionType    = selectionAction.getTypeAction().getCurrentIndex();
            const auto brushRadius      = selectionAction.getBrushRadiusAction().getValue();

            // Get the FBO size in floating point
            const auto fboSize = QSizeF(static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));

            // Configure shader program
            selectionToolOffScreenShaderProgram->setUniformValue("pixelSelectionTexture", 0);
            selectionToolOffScreenShaderProgram->setUniformValue("transform", transform);
            selectionToolOffScreenShaderProgram->setUniformValue("selectionType", selectionType);
            selectionToolOffScreenShaderProgram->setUniformValue("imageSize", fboSize.width(), fboSize.height());

            // Get number of mouse positions
            const auto numberOfMousePositions = mousePositions.size();

            switch (static_cast<PixelSelectionType>(selectionType))
            {
                case PixelSelectionType::Rectangle:
                {
                    if (numberOfMousePositions < 2)
                        break;

                    const auto rectangleTopLeft         = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.first());
                    const auto rectangleBottomRight     = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.last());
                    const auto rectangleTopLeftUV       = QVector2D(rectangleTopLeft.x() / fboSize.width(), rectangleTopLeft.y() / fboSize.height());
                    const auto rectangleBottomRightUV   = QVector2D(rectangleBottomRight.x() / fboSize.width(), rectangleBottomRight.y() / fboSize.height());
                    const auto rectangle                = QRectF(QPointF(rectangleTopLeftUV.x(), rectangleTopLeftUV.y()), QPointF(rectangleBottomRightUV.x(), rectangleBottomRightUV.y())).normalized();

                    selectionToolOffScreenShaderProgram->setUniformValue("rectangleTopLeft", rectangle.topLeft());
                    selectionToolOffScreenShaderProgram->setUniformValue("rectangleBottomRight", rectangle.bottomRight());

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                case PixelSelectionType::Brush:
                {
                    if (numberOfMousePositions <= 0)
                        break;

                    const auto brushCenter      = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, QPoint(0.0f, 0.0f));
                    const auto brushPerimeter   = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, QPoint(brushRadius, 0.0f));
                    const auto brushRadiusWorld = (brushPerimeter - brushCenter).length();

                    selectionToolOffScreenShaderProgram->setUniformValue("brushRadius", brushRadiusWorld);

                    if (numberOfMousePositions == 1) {
                        const auto brushCenter = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.last()).toVector2D();

                        selectionToolOffScreenShaderProgram->setUniformValue("previousBrushCenter", brushCenter);
                        selectionToolOffScreenShaderProgram->setUniformValue("currentBrushCenter", brushCenter);
                    }

                    if (numberOfMousePositions > 1) {
                        const auto previousBrushCenter  = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions[numberOfMousePositions - 2]).toVector2D();
                        const auto currentBrushCenter   = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.last()).toVector2D();

                        selectionToolOffScreenShaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter);
                        selectionToolOffScreenShaderProgram->setUniformValue("currentBrushCenter", currentBrushCenter);
                    }

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                case PixelSelectionType::Lasso:
                case PixelSelectionType::Polygon:
                {
                    if (numberOfMousePositions < 2)
                        break;

                    QList<QVector2D> points;

                    points.reserve(static_cast<std::int32_t>(numberOfMousePositions));

                    for (const auto& mousePosition : mousePositions)
                        points.push_back(getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePosition).toVector2D());

                    selectionToolOffScreenShaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
                    selectionToolOffScreenShaderProgram->setUniformValue("noPoints", static_cast<int>(points.size()));

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                case PixelSelectionType::Sample:
                {
                    if (numberOfMousePositions <= 0)
                        break;

                    // Convert sample 2D screen position to world position
                    QList<QVector2D> points{ getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.first()).toVector2D() };

                    // Assign sample point to shader
                    selectionToolOffScreenShaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                case PixelSelectionType::ROI:
                {
                    // Compute UV coordinates of viewing rectangle
                    const auto roiTopLeft       = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, QPoint(0, getRenderer().getParentWidgetSize().height()));
                    const auto roiBottomRight   = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, QPoint(getRenderer().getParentWidgetSize().width(), 0));
                    const auto roiTopLeftUV     = QVector2D(roiTopLeft.x() / fboSize.width(), roiTopLeft.y() / fboSize.height());
                    const auto roiBottomRightUV = QVector2D(roiBottomRight.x() / fboSize.width(), roiBottomRight.y() / fboSize.height());

                    QList<QVector2D> points {
                        roiTopLeftUV,
                        roiBottomRightUV
                    };

                    // Assign sample point to shader
                    selectionToolOffScreenShaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                default:
                    break;
            }

            // Release the shader program
            selectionToolOffScreenShaderProgram->release();

            // And shape VAO
            quadShape->getVAO().release();

            // Release the FBO
            _fbo->release();
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection prop off-screen image buffer computation failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop off-screen image buffer computation failed");
    }
}

void SelectionToolProp::resetOffScreenSelectionBuffer()
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Bind the FBO
            if (!_fbo->bind())
                throw std::runtime_error("Unable to bind FBO");

            // Clear the FBO
            glViewport(0, 0, _fbo->width(), _fbo->height());
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            // Release the FBO
            _fbo->release();
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection prop off-screen image buffer reset failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop off-screen image buffer reset failed");
    }
}

QImage SelectionToolProp::getSelectionImage()
{
    // Return empty image when the FBO is invalid
    if (_fbo.isNull())
        return QImage();

    getRenderer().bindOpenGLContext();

    // Convert the FBO to image and return
    return _fbo->toImage();
}

void SelectionToolProp::loadSelectionToolShaderProgram()
{
    // Load vertex/fragment shaders from resources
    const auto vertexShader     = loadFileContents(":Shaders/SelectionToolVertex.glsl");
    const auto fragmentShader   = loadFileContents(":Shaders/SelectionToolFragment.glsl");

    // Get selection tool shader program
    const auto selectionToolShaderProgram = getShaderProgramByName("SelectionTool");

    // Assign vertex shader to shader program
    if (!selectionToolShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        throw std::runtime_error("Unable to compile selection tool prop vertex shader");

    // Assign fragment shader to shader program
    if (!selectionToolShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        throw std::runtime_error("Unable to compile selection tool prop fragment shader");

    // Link the shader program
    if (!selectionToolShaderProgram->link())
        throw std::runtime_error("Unable to link selection tool prop shader program");

    // Number of bytes per stride
    const auto stride = 5 * sizeof(GLfloat);

    // Get quad shape
    auto shape = getShapeByName<QuadShape>("Quad");

    // Bind the shader program
    if (!selectionToolShaderProgram->bind())
        throw std::runtime_error("Unable to bind selection tool shader program");

    shape->getVAO().bind();
    {
        shape->getVBO().bind();
        {
            // Configure shader program
            selectionToolShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
            selectionToolShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
            selectionToolShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
            selectionToolShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
            selectionToolShaderProgram->release();
        }
        shape->getVAO().release();
    }
    shape->getVBO().release();
}

void SelectionToolProp::loadSelectionToolOffScreenShaderProgram()
{
    // Load vertex/fragment shaders from resources
    const auto vertexShader     = loadFileContents(":Shaders/SelectionToolOffScreenVertex.glsl");
    const auto fragmentShader   = loadFileContents(":Shaders/SelectionToolOffScreenFragment.glsl");

    // Get selection tool off-screen shader program
    const auto selectionToolOffScreenShaderProgram = getShaderProgramByName("SelectionToolOffScreen");

    // Assign vertex shader to shader program
    if (!selectionToolOffScreenShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        throw std::runtime_error("Unable to compile selection tool off-screen vertex shader");

    // Assign fragment shader to shader program
    if (!selectionToolOffScreenShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        throw std::runtime_error("Unable to compile selection tool off-screen fragment shader");

    // Link the shader program
    if (!selectionToolOffScreenShaderProgram->link())
        throw std::runtime_error("Unable to link selection tool off-screen shader program");

    // Number of bytes per stride
    const auto stride = 5 * sizeof(GLfloat);

    // Get quad shape
    auto shape = getShapeByName<QuadShape>("Quad");

    // Bind the shader program
    if (!selectionToolOffScreenShaderProgram->bind())
        throw std::runtime_error("Unable to bind selection tool off-screen shader program");

    shape->getVAO().bind();
    {
        shape->getVBO().bind();
        {
            // Configure shader program
            selectionToolOffScreenShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
            selectionToolOffScreenShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
            selectionToolOffScreenShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
            selectionToolOffScreenShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
            selectionToolOffScreenShaderProgram->release();
        }
        shape->getVAO().release();
    }
    shape->getVBO().release();
}