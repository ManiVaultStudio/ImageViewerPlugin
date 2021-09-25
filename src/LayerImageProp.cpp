#include "LayerImageProp.h"
#include "QuadShape.h"
#include "Range.h"
#include "Renderer.h"

#include "util/FileUtil.h"
#include "util/Interpolation.h"
#include "util/Exception.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept>

LayerImageProp::LayerImageProp(Layer& layer, const QString& name) :
    Prop(layer, name),
    _layer(layer),
    _displayRanges({ {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} })
{
    // Add quad shape and shader program
    addShape<QuadShape>("Quad");
    addShaderProgram("Quad");

    // Add color map and channels texture
    addTexture("ColorMap", QOpenGLTexture::Target2D);
    addTexture("Channels", QOpenGLTexture::Target2DArray);

    // Initialize the prop
    initialize();
}

void LayerImageProp::initialize()
{
    try
    {
        getRenderer().bindOpenGLContext();
        {
            Prop::initialize();

            const auto shaderProgram = getShaderProgramByName("Quad");

            // Load vertex/fragment shaders from resources
            const auto vertexShader     = loadFileContents(":/Shaders/PointsVertex.glsl");
            const auto fragmentShader   = loadFileContents(":Shaders/PointsFragment.glsl");

            // Assign vertex shader to shader program
            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
                throw std::runtime_error("Unable to compile quad vertex shader");

            // Assign fragment shader to shader program
            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
                throw std::runtime_error("Unable to compile quad fragment shader");

            // Link the shader program
            if (!shaderProgram->link())
                throw std::runtime_error("Unable to link quad shader program");

            const auto stride = 5 * sizeof(GLfloat);

            auto shape = getShapeByName<QuadShape>("Quad");

            // Create shader program
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

            // Create quad with image size
            setImageSize(_layer.getImageSize());

            _initialized = true;
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Layer image prop initialization failed", e);
    }
    catch (...) {
        exceptionMessageBox("Layer image prop initialization failed");
    }
}

void LayerImageProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
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

        if (shaderProgram->bind()) {
            auto& imageAction = _layer.getLayerAction().getImageAction();

            // Convert display ranges
            const QVector2D displayRanges[3] = {
                QVector2D(_displayRanges[0].first, _displayRanges[0].second),
                QVector2D(_displayRanges[1].first, _displayRanges[1].second),
                QVector2D(_displayRanges[2].first, _displayRanges[2].second)
            };

            // Configure shared program
            shaderProgram->setUniformValue("colorMapTexture", 0);
            shaderProgram->setUniformValue("channelTextures", 1);
            shaderProgram->setUniformValue("noChannels", imageAction.getNumberOfActiveChannels());
            shaderProgram->setUniformValue("useConstantColor", imageAction.getUseConstantColorAction().isChecked());
            shaderProgram->setUniformValue("constantColor", imageAction.getConstantColorAction().getColor());
            shaderProgram->setUniformValue("colorSpace", imageAction.getColorSpaceAction().getCurrentIndex());
            shaderProgram->setUniformValueArray("displayRanges", displayRanges, 3);
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
        exceptionMessageBox("Layer image prop rendering failed", e.what());
    }
    catch (...) {
        exceptionMessageBox("Layer image prop rendering failed");
    }
}

void LayerImageProp::setImageSize(const QSize& imageSize)
{
    try {
        // Compute quad rectangle
        const auto quadShapeRectangle = QRectF(QPointF(0.f, 0.f), QSizeF(imageSize));

        // Assign the rectangle to the quad shape
        getShapeByName<QuadShape>("Quad")->setRectangle(quadShapeRectangle);

        // Update the model matrix
        updateModelMatrix();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Set layer image prop image size failed", e.what());
    }
    catch (...) {
        exceptionMessageBox("Set layer image prop image size failed");
    }
}

void LayerImageProp::setColorMapImage(const QImage& colorMapImage)
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Make sure image is valid
            if (colorMapImage.isNull())
                return;

            // Get color map texture
            auto& texture = getTextureByName("ColorMap");

            // Reset and assign the texture
            texture.reset(new QOpenGLTexture(colorMapImage));

            // Create the texture if not created
            if (!texture->isCreated())
                texture->create();

            // Set appropriate wrapping
            texture->setWrapMode(QOpenGLTexture::ClampToEdge);
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set color map image in layer image prop", e.what());
    }
    catch (...) {
        exceptionMessageBox("Unable to set color map image in layer image prop");
    }
}

void LayerImageProp::setChannelScalarData(const std::uint32_t& channelIndex, const std::vector<float>& scalarData, const DisplayRange& displayRange)
{
    try {
        if (channelIndex > 3)
            throw std::runtime_error("Invalid channel index");

        getRenderer().bindOpenGLContext();
        {
            // Get image size from quad shape
            const auto imageSize = getShapeByName<QuadShape>("Quad")->getImageSize();

            // Only proceed if the image size is valid (non-zero in x/y)
            if (!imageSize.isValid())
                return;

            // Assign display range
            _displayRanges[channelIndex] = displayRange;

            // Get channels texture
            auto texture = getTextureByName("Channels");

            // Create the texture if not created
            if (!texture->isCreated())
                texture->create();

            // Re-configure when the image size has changed
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

            // Configure interpolation
            switch (_layer.getLayerAction().getImageAction().getInterpolationTypeAction().getCurrentIndex())
            {
                case static_cast<std::int32_t>(InterpolationType::Bilinear) :
                    texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
                    break;

                case static_cast<std::int32_t>(InterpolationType::NearestNeighbor) :
                    texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                    break;

                default:
                    break;
            }

            // Assign the scalar data to the texture
            texture->setData(0, channelIndex, QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, scalarData.data());
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set channel scalar data in layer image prop", e.what());
    }
    catch (...) {
        exceptionMessageBox("Unable to set channel scalar data in layer image prop");
    }
}

QRectF LayerImageProp::getBoundingRectangle() const
{
    return getShapeByName<QuadShape>("Quad")->getRectangle();
}

void LayerImageProp::updateModelMatrix()
{
    QMatrix4x4 modelMatrix;

    const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

    modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

    setModelMatrix(modelMatrix);
}