#include "SelectionBoundsRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

SelectionBoundsRenderer::SelectionBoundsRenderer() :
	Renderer(),
	_vertexData(),
	_vbo(),
	_vao(),
	_program(),
	_modelViewProjection(),
	_selectionBoundsColor(1.0f, 0.6f, 0.f, 0.5f),
	_selectionBounds()
{
	_vertexData.resize(20);
}

void SelectionBoundsRenderer::init()
{
	/*
	initializeOpenGLFunctions();

	_program = std::make_unique<QOpenGLShaderProgram>();

	// Shader program
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, imageVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
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
	*/
}

void SelectionBoundsRenderer::render()
{
	/*
	if (!initialized())
		return;

	_texture->bind();
	{
		_vao.bind();
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		_vao.release();
	}
	_texture->release();
	*/
}

void SelectionBoundsRenderer::destroy()
{
	//_texture->destroy();
	_vbo.destroy();
	_vao.destroy();
}

void SelectionBoundsRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;
}