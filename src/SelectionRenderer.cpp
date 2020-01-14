#include "SelectionRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

SelectionRenderer::SelectionRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_selectionColor(1.f, 0.f, 0.f, 0.6f)
{
}

void SelectionRenderer::render()
{
	if (!initialized())
		return;

	auto selectionProgram = shaderProgram("Selection");

	if (selectionProgram->bind()) {
		selectionProgram->setUniformValue("selectionTexture", 0);
		selectionProgram->setUniformValue("transform", _modelViewProjection);
		selectionProgram->setUniformValue("color", _selectionColor);

		auto selectionTexture = texture("Selection");

		selectionTexture->bind();
		{
			QuadRenderer::render();
		}
		selectionTexture->release();

		selectionProgram->release();
	}
}

void SelectionRenderer::setImage(std::shared_ptr<QImage> image)
{
	auto& selectionTexture = texture("Selection");

	selectionTexture.reset(new QOpenGLTexture(*image.get()));

	selectionTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	setSize(image->size());
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}

bool SelectionRenderer::initialized()
{
	return texture("Selection").get() != nullptr && texture("Selection")->isCreated();
}

void SelectionRenderer::initializeShaderPrograms()
{
	auto selectionProgram = std::make_shared<QOpenGLShaderProgram>();

	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	selectionProgram->link();

	_shaderPrograms.insert("Selection", selectionProgram);
}

void SelectionRenderer::initializeTextures()
{
	_textures.insert("Selection", std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D));
}