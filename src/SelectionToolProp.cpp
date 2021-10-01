#include "SelectionToolProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Renderer.h"
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

            // Update image size
            setImageSize(_layer.getImageSize());

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

        const auto shape            = getShapeByName<QuadShape>("Quad");
        const auto shaderProgram    = getShaderProgramByName("SelectionTool");

        // Bind shader program
        if (!shaderProgram->bind())
            throw std::runtime_error("Unable to bind quad shader program");

        glBindTexture(GL_TEXTURE_2D, _fbo->texture());

        // Get reference to selection action
        auto& selectionAction = _layer.getLayerAction().getSelectionAction();

        // Configure shader program
        shaderProgram->setUniformValue("offScreenTexture", 0);
        shaderProgram->setUniformValue("color", selectionAction.getOverlayColor().getColor());
        shaderProgram->setUniformValue("opacity", selectionAction.getOverlayOpacity().getValue());
        shaderProgram->setUniformValue("transform", modelViewProjectionMatrix * _renderable.getModelMatrix() * getModelMatrix());

        // Render the quad
        shape->render();

        // Release the shader program
        shaderProgram->release();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection tool prop rendering failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection tool prop rendering failed");
    }
}

void SelectionToolProp::setImageSize(const QSize& imageSize)
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Except if image size is invalid
            if (!imageSize.isValid())
                throw std::runtime_error("Image size not valid");

            auto createFbo = false;

            // Create FBO when none exists
            if (_fbo.isNull()) {
                createFbo = true;
            } else {
                if (imageSize != QSize(_fbo->width(), _fbo->height()))
                    createFbo = true;
            }

            // Create FBO when needed
            if (createFbo)
                _fbo.reset(new QOpenGLFramebufferObject(imageSize.width(), imageSize.height()));

            // Set rectangle
            this->getShapeByName<QuadShape>("Quad")->setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(imageSize)));

            // Update the model matrix
            updateModelMatrix();
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Set selection prop set image size failed", e);
    }
    catch (...) {
        exceptionMessageBox("Set selection prop set image size failed");
    }
}

void SelectionToolProp::compute(const QVector<QPoint>& mousePositions)
{
    // Only render if we have a valid FBO
    if (_fbo.isNull())
        return;

    try {
        getRenderer().bindOpenGLContext();
        {
            // Bind FBO for off-screen rendering
            if (!_fbo->bind())
                throw std::runtime_error("Unable to bind FBO");

            // Create viewport with the same size as the FBO
            glViewport(0, 0, _fbo->width(), _fbo->height());

            QMatrix4x4 transform;

            // Create orthogonal transformation matrix
            transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

            // Get reference to selection action
            auto& selectionAction = _layer.getLayerAction().getSelectionAction();

            // Get quad shape and compute the model-view-matrix
            auto shape              = getShapeByName<QuadShape>("Quad");
            auto modelViewMatrix    = _layer.getRenderer().getViewMatrix() * _renderable.getModelMatrix() * getModelMatrix();

            // Get shader program for the off-screen rendering
            const auto shaderProgram = getShaderProgramByName("SelectionToolOffScreen");

            // Bind the quad vertex array object buffer
            shape->getVAO().bind();

            // Bind shader program
            if (!shaderProgram->bind())
                throw std::runtime_error("Unable to bind off screen shader program");

            glBindTexture(GL_TEXTURE_2D, _fbo->texture());

            // Get selection type and brush radius
            const auto selectionType    = selectionAction.getTypeAction().getCurrentIndex();
            const auto brushRadius      = selectionAction.getBrushRadiusAction().getValue();

            // Configure shader program
            shaderProgram->setUniformValue("pixelSelectionTexture", 0);
            shaderProgram->setUniformValue("transform", transform);
            shaderProgram->setUniformValue("selectionType", selectionType);

            const auto fboSize                  = QSizeF(static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));
            const auto numberOfMousePositions    = mousePositions.size();

            shaderProgram->setUniformValue("imageSize", fboSize.width(), fboSize.height());

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

                    shaderProgram->setUniformValue("rectangleTopLeft", rectangle.topLeft());
                    shaderProgram->setUniformValue("rectangleBottomRight", rectangle.bottomRight());

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

                    shaderProgram->setUniformValue("brushRadius", brushRadiusWorld);

                    if (numberOfMousePositions == 1) {
                        const auto brushCenter = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.last()).toVector2D();

                        shaderProgram->setUniformValue("previousBrushCenter", brushCenter);
                        shaderProgram->setUniformValue("currentBrushCenter", brushCenter);
                    }

                    if (numberOfMousePositions > 1) {
                        const auto previousBrushCenter  = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions[numberOfMousePositions - 2]).toVector2D();
                        const auto currentBrushCenter   = getRenderer().getScreenPointToWorldPosition(modelViewMatrix, mousePositions.last()).toVector2D();

                        shaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter);
                        shaderProgram->setUniformValue("currentBrushCenter", currentBrushCenter);
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

                    shaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
                    shaderProgram->setUniformValue("noPoints", static_cast<int>(points.size()));

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
                    shaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));

                    // Draw off-screen 
                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                    break;
                }

                default:
                    break;
            }

            // Release the shader program
            shaderProgram->release();

            // And shape VAO
            shape->getVAO().release();

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

QRectF SelectionToolProp::getWorldBoundingRectangle() const
{
    return getShapeByName<QuadShape>("Quad")->getRectangle();
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
    const auto shaderProgram = getShaderProgramByName("SelectionTool");

    // Assign vertex shader to shader program
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        throw std::runtime_error("Unable to compile selection tool prop vertex shader");

    // Assign fragment shader to shader program
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        throw std::runtime_error("Unable to compile selection tool prop fragment shader");

    // Link the shader program
    if (!shaderProgram->link())
        throw std::runtime_error("Unable to link selection tool prop shader program");

    // Number of bytes per stride
    const auto stride = 5 * sizeof(GLfloat);

    // Get quad shape
    auto shape = getShapeByName<QuadShape>("Quad");

    // Bind the shader program
    if (!shaderProgram->bind())
        throw std::runtime_error("Unable to bind selection tool shader program");

    shape->getVAO().bind();
    {
        shape->getVBO().bind();
        {
            // Configure shader program
            shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
            shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
            shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
            shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
            shaderProgram->release();
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

    // Get selection tool shader program
    const auto shaderProgram = getShaderProgramByName("SelectionToolOffScreen");

    // Assign vertex shader to shader program
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
        throw std::runtime_error("Unable to compile selection tool off-screen vertex shader");

    // Assign fragment shader to shader program
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
        throw std::runtime_error("Unable to compile selection tool off-screen fragment shader");

    // Link the shader program
    if (!shaderProgram->link())
        throw std::runtime_error("Unable to link selection tool off-screen shader program");

    // Number of bytes per stride
    const auto stride = 5 * sizeof(GLfloat);

    // Get quad shape
    auto shape = getShapeByName<QuadShape>("Quad");

    // Bind the shader program
    if (!shaderProgram->bind())
        throw std::runtime_error("Unable to bind selection tool off-screen shader program");

    shape->getVAO().bind();
    {
        shape->getVBO().bind();
        {
            // Configure shader program
            shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
            shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
            shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
            shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
            shaderProgram->release();
        }
        shape->getVAO().release();
    }
    shape->getVBO().release();
}

void SelectionToolProp::updateModelMatrix()
{
    QMatrix4x4 modelMatrix;

    // Get quad shape
    const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

    // Compute the  model matrix
    modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

    // Assign model matrix
    setModelMatrix(modelMatrix);
}