#include "LayerImageProp.h"
#include "QuadShape.h"
#include "Range.h"
#include "Renderer.h"

#include "util/FileUtil.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept>

LayerImageProp::LayerImageProp(Layer& layer, const QString& name) :
    Prop(layer, name)
{
    addShape<QuadShape>("Quad");
    addShaderProgram("Quad");

    addTexture("ColorMap", QOpenGLTexture::Target2D);
    addTexture("Channels", QOpenGLTexture::Target2DArray);

    /*
    QObject::connect(pointsLayer, &PointsLayer::channelChanged, [this, pointsLayer](const std::uint32_t& channelId) {
        renderer->bindOpenGLContext();
        {
            auto channel = pointsLayer->getChannel(channelId);

            const auto imageSize = channel->getImageSize();

            if (!imageSize.isValid())
                return;

            auto texture = getTextureByName("Channels");

            if (!texture->isCreated())
                texture->create();

            if (imageSize != QSize(texture->width(), texture->height())) {
                texture->destroy();
                texture->create();
                texture->setLayers(4);
                texture->setSize(imageSize.width(), imageSize.height(), 1);
                texture->setSize(imageSize.width(), imageSize.height(), 2);
                texture->setSize(imageSize.width(), imageSize.height(), 3);
                texture->setSize(imageSize.width(), imageSize.height(), 4);
                texture->setFormat(QOpenGLTexture::R32F);
                texture->allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::Float32);
                texture->setWrapMode(QOpenGLTexture::ClampToEdge);
            }

            switch (pointsLayer->getInterpolationType(Qt::EditRole).toInt())
            {
                case static_cast<std::int32_t>(PointsLayer::InterpolationType::Bilinear):
                    texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
                    break;

                    case static_cast<std::int32_t>(PointsLayer::InterpolationType::NearestNeighbour) :
                        texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                        break;

                    default:
                        break;
            }

            texture->setData(0, channel->getId(), QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, channel->getElements().data());

            const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(imageSize));

            this->getShapeByName<QuadShape>("Quad")->setRectangle(rectangle);

            updateModelMatrix();
        }
        renderer->releaseOpenGLContext();
    });
    
    QObject::connect(pointsLayer, &PointsLayer::colorMapChanged, [this](const QImage& colorMap) {
        renderer->bindOpenGLContext();
        {
            if (colorMap.isNull())
                return;

            auto& texture = getTextureByName("ColorMap");
            
            texture.reset(new QOpenGLTexture(colorMap));

            if (!texture->isCreated())
                texture->create();

            texture->setWrapMode(QOpenGLTexture::ClampToEdge);
        }
        renderer->releaseOpenGLContext();
    });
    */

    initialize();
}

LayerImageProp::~LayerImageProp() = default;

void LayerImageProp::initialize()
{
    try
    {
        getRenderer().bindOpenGLContext();
        {
            Prop::initialize();

            const auto shaderProgram    = getShaderProgramByName("Quad");
            const auto vertexShader     = loadFileContents(":PointsVertex.glsl");
            const auto fragmentShader   = loadFileContents(":PointsFragment.glsl");

            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
                throw std::runtime_error("Unable to compile quad vertex shader");

            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
                throw std::runtime_error("Unable to compile quad fragment shader");

            if (!shaderProgram->link())
                throw std::runtime_error("Unable to link quad shader program");

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
                throw std::runtime_error("Unable to bind quad shader program");
            }

            _initialized = true;
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "initialization failed:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "initialization failed due to unhandled exception";
    }
}

void LayerImageProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
    //getRenderer().bindOpenGLContext();
    {
        getShapeByName<QuadShape>("Quad")->setRectangle(QRectF(0.0f, 0.0f, 10000.0f, 10000.0f));
        updateModelMatrix();
    }
    //getRenderer().releaseOpenGLContext();

    try {
        if (!canRender())
            return;
        
        Prop::render(nodeMVP, opacity);

        const auto shape            = getShapeByName<QuadShape>("Quad");
        const auto shaderProgram    = getShaderProgramByName("Quad");

        if (getTextureByName("ColorMap")->isCreated()) {
            getRenderer().getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
            getTextureByName("ColorMap")->bind();
        }

        if (getTextureByName("Channels")->isCreated()) {
            getRenderer().getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE1);
            getTextureByName("Channels")->bind();
        }

        //auto pointsLayer = static_cast<PointsLayer*>(_node);
        
        if (shaderProgram->bind()) {
            /*
            const QVector2D displayRanges[] = {
                pointsLayer->getChannel(0)->getDisplayRangeVector(),
                pointsLayer->getChannel(1)->getDisplayRangeVector(),
                pointsLayer->getChannel(2)->getDisplayRangeVector()
            };

            const auto noChannels       = pointsLayer->getNoChannels(Qt::EditRole).toInt();
            const auto useConstantColor = pointsLayer->getUseConstantColor(Qt::EditRole).toBool();
            const auto constantColor    = pointsLayer->getConstantColor(Qt::EditRole).value<QColor>();
            const auto colorSpace       = pointsLayer->getColorSpace(Qt::EditRole).toInt();
            */

            shaderProgram->setUniformValue("colorMapTexture", 0);
            shaderProgram->setUniformValue("channelTextures", 1);
            //shaderProgram->setUniformValue("noChannels", noChannels);
            //shaderProgram->setUniformValue("useConstantColor", useConstantColor);
            //shaderProgram->setUniformValue("constantColor", constantColor);
            //shaderProgram->setUniformValue("colorSpace", colorSpace);
            //shaderProgram->setUniformValueArray("displayRanges", displayRanges, 3);
            shaderProgram->setUniformValue("opacity", opacity);
            shaderProgram->setUniformValue("transform", nodeMVP * getModelMatrix());

            shape->render();

            shaderProgram->release();
        }
        else {
            throw std::runtime_error("Unable to bind quad shader program");
        }

        if (getTextureByName("Channels")->isCreated())
            getTextureByName("Channels")->release();

        if (getTextureByName("ColorMap")->isCreated())
            getTextureByName("ColorMap")->release();
    }
    catch (std::exception& e)
    {
        qDebug() << _name << "render failed:" << e.what();
    }
    catch (...) {
        qDebug() << _name << "render failed due to unhandled exception";
    }
}

QRectF LayerImageProp::getBoundingRectangle() const
{
    auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();
    /*
    rectangle.setSize(_node->getScale(Qt::EditRole).toFloat() * rectangle.size());
    */
    return rectangle;
}

void LayerImageProp::updateModelMatrix()
{
    QMatrix4x4 modelMatrix;

    const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

    modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

    setModelMatrix(modelMatrix);
}