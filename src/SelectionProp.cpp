#include "SelectionProp.h"
#include "QuadShape.h"
#include "LayersModel.h"
#include "LayersRenderer.h"
#include "ImageViewerPlugin.h"
#include "SettingsAction.h"

#include <util/FileUtil.h>
#include <util/Exception.h>

#include <QDebug>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPixelTransferOptions>

#include <stdexcept>

SelectionProp::SelectionProp(Layer& layer, const QString& name) :
    Prop(layer, name),
    _layer(layer)
{
    // Add quad shape and shader program
    addShape<QuadShape>("Quad");
    addShaderProgram("Quad");

    // Add channels texture
    addTexture("Textures", QOpenGLTexture::Target2DArray);

    // Initialize the prop
    initialize();
}

void SelectionProp::initialize()
{
    try
    {
        Prop::initialize();

        getRenderer().bindOpenGLContext();
        {
            const auto shaderProgram = getShaderProgramByName("Quad");

            // Load vertex/fragment shaders from resources
            const auto vertexShader     = loadFileContents(":/Shaders/SelectionVertex.glsl");
            const auto fragmentShader   = loadFileContents(":Shaders/SelectionFragment.glsl");

            // Assign vertex shader to shader program
            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader))
                throw std::runtime_error("Unable to compile quad vertex shader");

            // Assign fragment shader to shader program
            if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader))
                throw std::runtime_error("Unable to compile quad fragment shader");

            // Link the shader program
            if (!shaderProgram->link())
                throw std::runtime_error("Unable to link quad shader program");

            // Number of bytes per stride
            const auto stride = 5 * sizeof(GLfloat);

            // Get quad shape
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
        exceptionMessageBox("Selection prop initialization failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop initialization failed");
    }
}

void SelectionProp::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    try {

        if (!canRender())
            return;

        const auto shape            = getShapeByName<QuadShape>("Quad");
        const auto shaderProgram    = getShaderProgramByName("Quad");
        const auto quadTexture      = getTextureByName("Quad");

        // Activate and bind selection texture
        if (getTextureByName("Textures")->isCreated()) {
            getRenderer().getOpenGLContext()->functions()->glActiveTexture(GL_TEXTURE0);
            getTextureByName("Textures")->bind();
        }
        else {
            throw std::runtime_error("Texture not created.");
        }

        // Bind the selection texture
        getTextureByName("Textures")->bind();

        // Bind shader program
        if (!shaderProgram->bind())
            throw std::runtime_error("Unable to bind shader program");

        // Get reference to selection action
        auto& selectionAction = _layer.getSelectionAction();

        // Configure shader program
        shaderProgram->setUniformValue("textures", 0);
        shaderProgram->setUniformValue("overlayColor", selectionAction.getPixelSelectionAction().getOverlayColorAction().getColor());
        shaderProgram->setUniformValue("opacity", 0.01f * selectionAction.getPixelSelectionAction().getOverlayOpacityAction().getValue());
        shaderProgram->setUniformValue("transform", modelViewProjectionMatrix * _renderable.getModelMatrix() * getModelMatrix());

        // Render the quad
        shape->render();

        // Release the shader program
        shaderProgram->release();

        // Release texture
        getTextureByName("Textures")->release();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection prop rendering failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop rendering failed");
    }
}

QRectF SelectionProp::getWorldBoundingRectangle() const
{
    // Get quad bounding rectangle
    auto boundingRectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

    // Compute composite matrix
    const auto matrix = _renderable.getModelMatrix() * getModelMatrix();

    // Compute rectangle extents in world coordinates
    const auto worldTopLeft     = matrix * boundingRectangle.topLeft();
    const auto worldBottomRight = matrix * boundingRectangle.bottomRight();

    return QRectF(worldTopLeft, worldBottomRight);
}

void SelectionProp::setGeometry(const QRectF& imageRectangle)
{
    try {
        qDebug() << "Set selection prop geometry:" << imageRectangle;

        // Assign the rectangle to the quad shape
        getShapeByName<QuadShape>("Quad")->setRectangle(imageRectangle);

        // Update the model matrix
        QMatrix4x4 modelMatrix;

        // Get quad shape
        const auto rectangle = getShapeByName<QuadShape>("Quad")->getRectangle();

        // Establish center
        const auto center = imageRectangle.center();

        // Compute the  model matrix
        modelMatrix.translate(-center.x(), -center.y(), 0.0f);

        // Assign model matrix
        setModelMatrix(modelMatrix);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Selection prop set geometry failed", e);
    }
    catch (...) {
        exceptionMessageBox("Selection prop set geometry failed");
    }
}

void SelectionProp::setSelectionData(const std::vector<std::uint8_t>& selectionData)
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Get image size from quad shape
            const auto imageSize = getShapeByName<QuadShape>("Quad")->getImageSize();

            // Only proceed if the image size is valid (non-zero in x/y)
            if (!imageSize.isValid())
                return;

            // Get channels texture
            auto texture = getTextureByName("Textures");

            // Create the texture if not created
            if (!texture->isCreated())
                texture->create();

            // Re-configure when the image size has changed
            if (imageSize != QSize(texture->width(), texture->height())) {
                texture->destroy();
                texture->create();
                texture->setLayers(2);
                texture->setSize(imageSize.width(), imageSize.height(), 1);
                texture->setSize(imageSize.width(), imageSize.height(), 2);
                texture->setFormat(QOpenGLTexture::R8_UNorm);
                texture->setWrapMode(QOpenGLTexture::ClampToEdge);
                texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                texture->allocateStorage();
            }

            QOpenGLPixelTransferOptions options;

            options.setAlignment(1);

            // Assign texture data
            texture->setData(0, 0, QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, selectionData.data(), &options);
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set scalar data in selection prop", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to set scalar data in selection prop");
    }
}

void SelectionProp::setMaskData(const std::vector<std::uint8_t>& maskData)
{
    try {
        getRenderer().bindOpenGLContext();
        {
            // Get image size from quad
            const auto imageSize = getShapeByName<QuadShape>("Quad")->getRectangle().size();

            // Only proceed if the image size is valid (non-zero in x/y)
            if (!imageSize.isValid())
                return;

            // Get channels texture
            auto texture = getTextureByName("Textures");

            // Create the texture if not created
            if (!texture->isCreated())
                texture->create();

            // Re-configure when the image size has changed
            if (imageSize != QSize(texture->width(), texture->height())) {
                texture->destroy();
                texture->create();
                texture->setLayers(2);
                texture->setSize(imageSize.width(), imageSize.height(), 1);
                texture->setSize(imageSize.width(), imageSize.height(), 2);
                texture->setFormat(QOpenGLTexture::R8_UNorm);
                texture->setWrapMode(QOpenGLTexture::ClampToEdge);
                texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
                texture->allocateStorage();
            }

            QOpenGLPixelTransferOptions options;

            options.setAlignment(1);

            // Assign the scalar data to the texture
            texture->setData(0, 1, QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt8, maskData.data(), &options);
        }
        getRenderer().releaseOpenGLContext();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to set mask data in layer selection prop", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to set mask data in layer selection prop");
    }
}
