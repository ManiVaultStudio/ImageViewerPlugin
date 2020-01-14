#include "SelectionRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

SelectionRenderer::SelectionRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_selectionColor(1.f, 0.f, 0.f, 0.6f)
{
}

void SelectionRenderer::init()
{
	QuadRenderer::init();

	const auto quadProgram = shaderProgram("Quad");

	if (quadProgram->bind()) {
		auto quadVBO = vbo("Quad");
		auto quadVAO = vao("Quad");

		quadVAO->bind();
		quadVBO->bind();

		const auto stride = 5 * sizeof(GLfloat);

		quadProgram->enableAttributeArray(QuadRenderer::_quadVertexAttribute);
		quadProgram->enableAttributeArray(QuadRenderer::_quadTextureAttribute);
		quadProgram->setAttributeBuffer(QuadRenderer::_quadVertexAttribute, GL_FLOAT, 0, 3, stride);
		quadProgram->setAttributeBuffer(QuadRenderer::_quadTextureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVAO->release();
		quadVBO->release();

		quadProgram->release();
	}
}

void SelectionRenderer::render()
{
	if (!isInitialized())
		return;

	auto quadProgram = shaderProgram("Quad");

	if (quadProgram->bind()) {
		quadProgram->setUniformValue("selectionTexture", 0);
		quadProgram->setUniformValue("transform", _modelViewProjection);
		quadProgram->setUniformValue("color", _selectionColor);

		auto& selectionTexture = texture("Quad");

		selectionTexture->bind();
		{
			QuadRenderer::render();
		}
		selectionTexture->release();

		quadProgram->release();
	}
}

void SelectionRenderer::setImage(std::shared_ptr<QImage> image)
{
	auto& selectionTexture = texture("Quad");

	selectionTexture.reset(new QOpenGLTexture(*image.get()));

	selectionTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	setSize(image->size());
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}

bool SelectionRenderer::isInitialized() const
{
	return texture("Quad").get() != nullptr && texture("Quad")->isCreated();
}

void SelectionRenderer::createShaderPrograms()
{
	auto quadProgram = std::make_shared<QOpenGLShaderProgram>();

	quadProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	quadProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	quadProgram->link();

	_shaderPrograms.insert("Quad", quadProgram);
}

void SelectionRenderer::createTextures()
{
	_textures.insert("Quad", std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D));
}