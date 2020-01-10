#include "SelectionRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectionRenderer::SelectionRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_selectionColor(1.f, 0.f, 0.f, 0.6f)
{
}

void SelectionRenderer::render()
{
	if (!initialized())
		return;

	_program->bind();
	{
		_program->setUniformValue("selectionTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);
		_program->setUniformValue("color", _selectionColor);

		QuadRenderer::render();
	}
	_program->release();
}

void SelectionRenderer::initializeProgram()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	_program->link();
}

void SelectionRenderer::setImage(std::shared_ptr<QImage> image)
{
	_texture.reset(new QOpenGLTexture(*image.get()));

	_texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	createQuad();
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}