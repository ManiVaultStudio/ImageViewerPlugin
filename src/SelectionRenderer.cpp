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

void SelectionRenderer::init()
{
	initializeOpenGLFunctions();

	_program = std::make_unique<QOpenGLShaderProgram>();

	// Shader program
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	
	_program->link();
	
	// Vertex buffer object
	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	// Vertex array object
	_vao.create();

	_program->bind();

	_vao.bind();
	{
		_vbo.bind();

		_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
		_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
		_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
		_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
	}
	_vao.release();
	_vbo.release();
	_program->release();
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

void SelectionRenderer::setImage(std::shared_ptr<QImage> image)
{
	_texture.reset(new QOpenGLTexture(*image.get()));

	createQuad();
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}