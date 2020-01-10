#include "SelectionRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

SelectionRenderer::SelectionRenderer(const std::uint32_t& zIndex) :
	QuadRenderer(zIndex),
	_texture(),
	_selectionColor(1.f, 0.f, 0.f, 0.6f)
{
}

void SelectionRenderer::render()
{
	if (!initialized())
		return;

	if (_program->bind()) {
		_program->setUniformValue("selectionTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);
		_program->setUniformValue("color", _selectionColor);

		_texture->bind();
		{
			QuadRenderer::render();
		}
		_texture->release();

		_program->release();
	}
}

void SelectionRenderer::initializePrograms()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	_program->link();
}

void SelectionRenderer::setImage(std::shared_ptr<QImage> image)
{
	_texture.reset(new QOpenGLTexture(*image.get()));

	_texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	setSize(image->size());
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}

bool SelectionRenderer::initialized() const
{
	return _texture.get() != nullptr && _texture->isCreated();
}