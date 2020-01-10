#include "QuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

QuadRenderer::QuadRenderer(const std::uint32_t& zIndex) :
	StackedRenderer(zIndex),
	_size(),
	_vertexData(),
	_vbo(),
	_vao(),
	_program(std::make_unique<QOpenGLShaderProgram>())
{
	_vertexData.resize(20);
}

void QuadRenderer::init()
{
	initializeOpenGLFunctions();
	initializePrograms();

	_vbo.create();
	_vbo.bind();
	_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_vbo.release();

	_vao.create();

	_program->bind();
	_vao.bind();
	_vbo.bind();

	const auto stride = 5 * sizeof(GLfloat);

	_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, stride);
	_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

	_vao.release();
	_vbo.release();
	_program->release();
}

void QuadRenderer::resize(QSize renderSize)
{
}

void QuadRenderer::render()
{
	if (!initialized())
		return;
	
	_vao.bind();
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	_vao.release();
}

void QuadRenderer::destroy()
{
	_vbo.destroy();
	_vao.destroy();
}

QSize QuadRenderer::size() const
{
	return _size;
}

void QuadRenderer::setSize(const QSize& size)
{
	if (size == _size)
		return;

	_size = size;

	createQuad();
}

void QuadRenderer::createQuad()
{
	qDebug() << "Create quad";

	const float width	= initialized() ? static_cast<float>(_size.width()) : 0;
	const float height	= initialized() ? static_cast<float>(_size.height()) : 0;

	qDebug() << width << height;

	const float coordinates[4][3] = {
	  { width, height, 0.0f },
	  { 0.0f, height, 0.0f },
	  { 0.0f, 0.0f, 0.0f },
	  { width, 0.0f, 0.0f }
	};

	for (int j = 0; j < 4; ++j)
	{
		_vertexData[j * 5 + 0] = coordinates[j][0];
		_vertexData[j * 5 + 1] = coordinates[j][1];
		_vertexData[j * 5 + 2] = coordinates[j][2];

		_vertexData[j * 5 + 3] = j == 0 || j == 3;
		_vertexData[j * 5 + 4] = j == 2 || j == 3;
	}

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		}
		_vbo.release();
	}
	_vao.release();
}