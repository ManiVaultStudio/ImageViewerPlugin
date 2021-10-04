#include "ImageProp.h"
#include "QuadShape.h"
#include "Renderer.h"

#include "util/FileUtil.h"
#include "util/Interpolation.h"
#include "util/Exception.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include <stdexcept>

ImageProp::ImageProp(Layer& layer, const QString& name) :
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

void ImageProp::initialize()
{
    try
    {
        Prop::initialize();

        getRenderer().bindOpenGLContext();
        {
            const auto shaderProgram = getShaderProgramByName("Quad");

            // Load vertex/fragment shaders from resources
            const auto vertexShader     = loadFileContents(":Shaders/ImageVertex.glsl");
            const auto fragmentShader   = loadFileContents(":Shaders/ImageFragment.glsl");

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

            // Bind shader program
            if (!shaderProgram->bind())
                throw std::runtime_error("Unable to bind quad shader program");
            
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

void ImageProp::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        if (!canRender())
            return;
        
        const auto shape            = getShapeByName<QuadShape>("Quad");
        const auto shaderProgram    = getShaderProgramByName("Quad");

        // Activate and bind color map texture
        if (getTextureByName("ColorMap")->isCreated()) {
            getRenderer().getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
            getTextureByName("ColorMap")->bind();
        }
        else {
            throw std::runtime_error("Color map texture is not created.");
        }

        // Activate and bind channels texture
        if (getTextureByName("Channels")->isCreated()) {
            getRenderer().getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE1);
            getTextureByName("Channels")->bind();
        }
        else {
            throw std::runtime_error("Channels texture is not created.");
        }

        // Bind shader program
        if (!shaderProgram->bind())
            throw std::runtime_error("Unable to bind quad shader program");

        auto& imageAction = _layer.getLayerAction().getImageAction();

        // Convert display ranges
        const QVector2D displayRanges[3] = {
            QVector2D(_displayRanges[0].first, _displayRanges[0].second),
            QVector2D(_displayRanges[1].first, _displayRanges[1].second),
            QVector2D(_displayRanges[2].first, _displayRanges[2].second)
        };

        // Configure shader program
        shaderProgram->setUniformValue("colorMapTexture", 0);
        shaderProgram->setUniformValue("channelTextures", 1);
        shaderProgram->setUniformValue("noChannels", imageAction.getNumberOfActiveChannels());
        shaderProgram->setUniformValue("useConstantColor", imageAction.getUseConstantColorAction().isChecked());
        shaderProgram->setUniformValue("constantColor", imageAction.getConstantColorAction().getColor());
        shaderProgram->setUniformValue("colorSpace", imageAction.getColorSpaceAction().getCurrentIndex());
        shaderProgram->setUniformValueArray("displayRanges", displayRanges, 3);
        shaderProgram->setUniformValue("opacity", 0.01f * imageAction.getOpacityAction().getValue());
        shaderProgram->setUniformValue("transform", modelViewProjectionMatrix * _renderable.getModelMatrix() * getModelMatrix());

        // Render the quad
        shape->render();

        // Release the shader program
        shaderProgram->release();

        // Release textures
        getTextureByName("Channels")->release();
        getTextureByName("ColorMap")->release();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Layer image prop rendering failed", e);
    }
    catch (...) {
        exceptionMessageBox("Layer image prop rendering failed");
    }
}

void ImageProp::setColorMapImage(const QImage& colorMapImage)
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
        exceptionMessageBox("Unable to set color map image in layer image prop", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to set color map image in layer image prop");
    }
}

void ImageProp::setChannelScalarData(const std::uint32_t& channelIndex, const QRect& sourceImageRectangle, const QRect& targetImageRectangle, const QSize& imageSize, const QVector<float>& scalarData, const DisplayRange& displayRange)
{
    try {
        if (channelIndex > 3)
            throw std::runtime_error("Invalid channel index");

        getRenderer().bindOpenGLContext();
        {
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

            // Set the interpolation type
            setInterpolationType(static_cast<InterpolationType>(_layer.getLayerAction().getImageAction().getInterpolationTypeAction().getCurrentIndex()));

            // Assign the scalar data to the texture
            texture->setData(0, channelIndex, QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::Float32, scalarData.data());

            // Compute quad rectangle

            // Assign the rectangle to the quad shape
            getShapeByName<QuadShape>("Quad")->setRectangle(targetImageRectangle);

            // Update the model matrix
            QMatrix4x4 modelMatrix;

            // Get quad shape
            const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

            // Compute the  model matrix
            modelMatrix.translate(-sourceImageRectangle.center().x(), -sourceImageRectangle.center().y(), 0.0f);

            // Assign model matrix
            setModelMatrix(modelMatrix);
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set channel scalar data in layer image prop", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to set channel scalar data in layer image prop");
    }
}

void ImageProp::setInterpolationType(const InterpolationType& interpolationType)
{
    try {
        // Get channels texture
        auto texture = getTextureByName("Channels");

        // Except when texture is not created
        if (!texture->isCreated())
            throw std::runtime_error("Channels texture is not created.");

        // Configure interpolation
        switch (interpolationType)
        {
            case InterpolationType::Bilinear :
                texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
                break;

            case InterpolationType::NearestNeighbor :
                texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                break;

            default:
                break;
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set channel interpolation type in layer image prop", e.what());
    }
    catch (...) {
        exceptionMessageBox("Unable to set channel interpolation type in layer image prop");
    }
}

QRectF ImageProp::getWorldBoundingRectangle() const
{
    // Get quad bounding rectangle
    auto boundingRectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

    // Compute composite matrix
    const auto matrix = _renderable.getModelMatrix() * getModelMatrix();

    // Compute rectangle extents in world coordinates
    const auto worldTopLeft         = matrix * boundingRectangle.topLeft();
    const auto worldBottomRight     = matrix * boundingRectangle.bottomRight();

    return QRectF(worldTopLeft, worldBottomRight);
}