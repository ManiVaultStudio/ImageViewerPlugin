#include "SelectionRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectionRenderer::SelectionRenderer() :
	QuadRenderer(),
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
		_program->setUniformValue("matrix", _modelViewProjection);
		_program->setUniformValue("color", _selectionColor);
		_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

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

	createQuad();
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}