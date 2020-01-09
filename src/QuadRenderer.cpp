#include "QuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

QuadRenderer::QuadRenderer() :
	Renderer(),
	_texture(),
	_vertexData(),
	_vbo(),
	_vao(),
	_program(),
	_image(),
	_modelViewProjection()
{
	_vertexData.resize(20);
}

void QuadRenderer::init()
{
	qDebug() << "QuadRenderer::init";

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
}

void QuadRenderer::resize(QSize renderSize)
{
	qDebug() << "QuadRenderer::resize";
}

void QuadRenderer::render()
{
	//qDebug() << "QuadRenderer::render" << _modelViewProjection;

	if (!initialized())
		return;

	_texture->bind();
	_vao.bind();

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	_vao.release();
	_texture->release();
}

void QuadRenderer::destroy()
{
	qDebug() << "QuadRenderer::destroy";

	_texture->destroy();
	_vbo.destroy();
	_vao.destroy();
}

void QuadRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;
}

QSize QuadRenderer::size() const
{
	return _image.get() == nullptr ? QSize() : _image->size();
}

bool QuadRenderer::initialized() const
{
	return _texture.get() != nullptr && _texture->isCreated();
}

void QuadRenderer::createQuad()
{
	const float width	= initialized() ? static_cast<float>(_texture->width()) : 0;
	const float height	= initialized() ? static_cast<float>(_texture->height()) : 0;

	qDebug() << "Create quad" << width << height;

	const float coordinates[4][3] = {
	  { width, height, 0.0f },
	  { 0.0f, height, 0.0f },
	  { 0.0f, 0.0f, 0.0f },
	  { width, 0.0f, 0.0f }
	};

	for (int j = 0; j < 4; ++j)
	{
		// Vertex position
		_vertexData[j * 5 + 0] = 1.0 * coordinates[j][0];
		_vertexData[j * 5 + 1] = 1.0 * coordinates[j][1];
		_vertexData[j * 5 + 2] = 1.0 * coordinates[j][2];

		// Texture coordinate
		_vertexData[j * 5 + 3] = j == 0 || j == 3;
		_vertexData[j * 5 + 4] = j == 2 || j == 3;
	}

	_vao.bind();
	{
		_vbo.bind();
		_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		_vbo.release();
	}
	_vao.release();
}