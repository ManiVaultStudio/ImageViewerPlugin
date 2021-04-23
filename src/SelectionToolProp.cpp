#include "SelectionToolProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "Range.h"
#include "Renderer.h"
#include "SelectionLayer.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept> // For runtime_error.

const std::string selectionToolVertexShaderSource =
    #include "SelectionToolVertex.glsl"
;

const std::string selectionToolFragmentShaderSource =
    #include "SelectionToolFragment.glsl"
;

const std::string selectionToolOffScreenVertexShaderSource =
    #include "SelectionToolOffScreenVertex.glsl"
;

const std::string selectionToolOffScreenFragmentShaderSource =
    #include "SelectionToolOffScreenFragment.glsl"
;

SelectionToolProp::SelectionToolProp(SelectionLayer* selectionLayer, const QString& name) :
    Prop(reinterpret_cast<Node*>(selectionLayer), name),
    _fbo()
{
    addShape<QuadShape>("Quad");

    addShaderProgram("SelectionTool");
    addShaderProgram("SelectionToolOffScreen");
    
    QObject::connect(selectionLayer, &SelectionLayer::channelChanged, [this, selectionLayer](const std::uint32_t& channelId) {
        try
        {
            renderer->bindOpenGLContext();

            if (channelId != 0)
                return;

            auto channel = selectionLayer->getChannel(channelId);

            const auto imageSize = channel->getImageSize();

            if (!imageSize.isValid())
                return;

            auto createFbo = false;

            if (_fbo.isNull())
                createFbo = true;

            if (!_fbo.isNull() && imageSize != QSize(_fbo->width(), _fbo->height()))
                createFbo = true;

            if (createFbo)
                _fbo.reset(new QOpenGLFramebufferObject(imageSize.width(), imageSize.height()));

            const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(imageSize));

            this->getShapeByName<QuadShape>("Quad")->setRectangle(rectangle);

			updateModelMatrix();
        }
        catch (std::exception& e)
        {
            qDebug() << _name << "frame buffer object update failed:" << e.what();
        }
        catch (...) {
            qDebug() << _name << "frame buffer object update failed due to unhandled exception";
        }
    });

    initialize();
}

SelectionToolProp::~SelectionToolProp() = default;

void SelectionToolProp::initialize()
{
    try
    {
        renderer->bindOpenGLContext();

        Prop::initialize();

        loadSelectionToolShaderProgram();
        loadSelectionToolOffScreenShaderProgram();

        _initialized = true;
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "initialization failed:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "initialization failed due to unhandled exception";
    }
}

void SelectionToolProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
    try {
        if (!canRender())
            return;

        Prop::render(nodeMVP, opacity);

        const auto shape            = getShapeByName<QuadShape>("Quad");
        const auto shaderProgram    = getShaderProgramByName("SelectionTool");

        if (shaderProgram->bind()) {
            glBindTexture(GL_TEXTURE_2D, _fbo->texture());

            auto selectionLayer = static_cast<SelectionLayer*>(_node);

            const auto overlayColor = selectionLayer->getOverlayColor(Qt::EditRole).value<QColor>();

            shaderProgram->setUniformValue("offScreenTexture", 0);
            shaderProgram->setUniformValue("color", SelectionLayer::fillColor);
            shaderProgram->setUniformValue("opacity", opacity);
			shaderProgram->setUniformValue("transform", nodeMVP * getModelMatrix());

            shape->render();

            shaderProgram->release();
        }
        else {
            throw std::runtime_error("Unable to bind quad shader program");
        }
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "render failed:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "render failed due to unhandled exception";
    }
}

void SelectionToolProp::compute()
{
    if (_fbo.isNull())
        return;

    //qDebug() << "Computing pixel selection";

    try {
        renderer->bindOpenGLContext();

        if (!_fbo->bind())
            throw std::runtime_error("Unable to bind frame buffer object");

        glViewport(0, 0, _fbo->width(), _fbo->height());

        QMatrix4x4 transform;

        transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

        auto shape = getShapeByName<QuadShape>("Quad");

        auto selectionLayer = static_cast<SelectionLayer*>(_node);
        auto modelViewMatrix = selectionLayer->getModelViewMatrix() * getModelMatrix();

        const auto shaderProgram = getShaderProgramByName("SelectionToolOffScreen");

        shape->getVAO().bind();
        
        if (shaderProgram->bind()) {
            glBindTexture(GL_TEXTURE_2D, _fbo->texture());

            const auto selectionType = selectionLayer->getPixelSelectionType(Qt::EditRole).toInt();

            shaderProgram->setUniformValue("pixelSelectionTexture", 0);
            shaderProgram->setUniformValue("transform", transform);
            shaderProgram->setUniformValue("selectionType", selectionType);

            const auto fboSize = QSizeF(static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));
            const auto mouseEvents = selectionLayer->getMousePositions();
            const auto noMouseEvents = mouseEvents.size();

            shaderProgram->setUniformValue("imageSize", fboSize.width(), fboSize.height());

            switch (static_cast<SelectionType>(selectionType))
            {
                case SelectionType::Rectangle:
                {
                    if (noMouseEvents < 2)
                        break;

                    const auto rectangleTopLeft = renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvents.first());
                    const auto rectangleBottomRight = renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvents.last());
                    const auto rectangleTopLeftUV = QVector2D(rectangleTopLeft.x() / fboSize.width(), rectangleTopLeft.y() / fboSize.height());
                    const auto rectangleBottomRightUV = QVector2D(rectangleBottomRight.x() / fboSize.width(), rectangleBottomRight.y() / fboSize.height());
                    const auto rectangle = QRectF(QPointF(rectangleTopLeftUV.x(), rectangleTopLeftUV.y()), QPointF(rectangleBottomRightUV.x(), rectangleBottomRightUV.y())).normalized();

                    shaderProgram->setUniformValue("rectangleTopLeft", rectangle.topLeft());
                    shaderProgram->setUniformValue("rectangleBottomRight", rectangle.bottomRight());

                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    break;
                }

                case SelectionType::Brush:
                {
                    if (noMouseEvents <= 0)
                        break;

                    const auto brushCenter = renderer->getScreenPointToWorldPosition(modelViewMatrix, QPoint(0.0f, 0.0f));
                    const auto brushPerimeter = renderer->getScreenPointToWorldPosition(modelViewMatrix, QPoint(selectionLayer->getBrushRadius(Qt::EditRole).toFloat(), 0.0f));
                    const auto brushRadiusWorld = (brushPerimeter - brushCenter).length();

                    shaderProgram->setUniformValue("brushRadius", brushRadiusWorld);

                    if (noMouseEvents == 1) {
                        const auto brushCenter = renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvents.last()).toVector2D();

                        shaderProgram->setUniformValue("previousBrushCenter", brushCenter);
                        shaderProgram->setUniformValue("currentBrushCenter", brushCenter);
                    }

                    if (noMouseEvents > 1) {
                        const auto previousBrushCenter = renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvents[noMouseEvents - 2]).toVector2D();
                        const auto currentBrushCenter = renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvents.last()).toVector2D();

                        shaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter);
                        shaderProgram->setUniformValue("currentBrushCenter", currentBrushCenter);
                    }

                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    break;
                }

                case SelectionType::Lasso:
                case SelectionType::Polygon:
                {
                    if (noMouseEvents < 2)
                        break;

                    QList<QVector2D> points;

                    points.reserve(static_cast<std::int32_t>(noMouseEvents));

                    for (const auto& mouseEvent : mouseEvents)
                        points.push_back(renderer->getScreenPointToWorldPosition(modelViewMatrix, mouseEvent).toVector2D());

                    shaderProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
                    shaderProgram->setUniformValue("noPoints", static_cast<int>(points.size()));

                    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
                    break;
                }

                case SelectionType::Sample:
                    break;

                default:
                    break;
            }

            shaderProgram->release();
        }
        else
        {
            throw std::runtime_error("Unable to bind off screen shader program");
        }

        shape->getVAO().release();

        _fbo->release();
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "compute pixel selection:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "compute pixel selection failed due to unhandled exception";
    }
}

void SelectionToolProp::reset()
{
    try {
        renderer->bindOpenGLContext();
        if (_fbo->bind()) {
            glViewport(0, 0, _fbo->width(), _fbo->height());
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            _fbo->release();
        }
        else
        {
            throw std::runtime_error("Unable to bind frame buffer object");
        }
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "reset failed:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "reset failed due to unhandled exception";
    }
}

QRectF SelectionToolProp::getBoundingRectangle() const
{
    return getShapeByName<QuadShape>("Quad")->getRectangle();
}

QImage SelectionToolProp::getSelectionImage()
{
    if (_fbo.isNull())
        return QImage();

    renderer->bindOpenGLContext();

    return _fbo->toImage();
}

void SelectionToolProp::loadSelectionToolShaderProgram()
{
    const auto shaderProgram = getShaderProgramByName("SelectionTool");

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionToolVertexShaderSource.c_str()))
        throw std::runtime_error("Unable to compile selection tool vertex shader");

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionToolFragmentShaderSource.c_str()))
        throw std::runtime_error("Unable to compile selection tool fragment shader");

    if (!shaderProgram->link())
        throw std::runtime_error("Unable to link selection tool shader program");

    const auto stride = 5 * sizeof(GLfloat);

    auto shape = getShapeByName<QuadShape>("Quad");

    if (shaderProgram->bind()) {
        shape->getVAO().bind();
        shape->getVBO().bind();

        shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
        shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
        shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
        shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
        shaderProgram->release();

        shape->getVAO().release();
        shape->getVBO().release();
    }
    else {
        throw std::runtime_error("Unable to bind selection tool shader program");
    }
}

void SelectionToolProp::loadSelectionToolOffScreenShaderProgram()
{
    const auto shaderProgram = getShaderProgramByName("SelectionToolOffScreen");

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionToolOffScreenVertexShaderSource.c_str()))
        throw std::runtime_error("Unable to compile selection tool off-screen vertex shader");

    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionToolOffScreenFragmentShaderSource.c_str()))
        throw std::runtime_error("Unable to compile selection tool off-screen fragment shader");

    if (!shaderProgram->link())
        throw std::runtime_error("Unable to link selection tool off-screen shader program");

    const auto stride = 5 * sizeof(GLfloat);

    auto shape = getShapeByName<QuadShape>("Quad");

    if (shaderProgram->bind()) {
        shape->getVAO().bind();
        shape->getVBO().bind();

        shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
        shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
        shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
        shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
        shaderProgram->release();

        shape->getVAO().release();
        shape->getVBO().release();
    }
    else {
        throw std::runtime_error("Unable to bind selection tool off-screen shader program");
    }
}

void SelectionToolProp::updateModelMatrix()
{
	QMatrix4x4 modelMatrix;

	const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);
}