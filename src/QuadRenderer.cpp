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
	_quadVBO(),
	_quadVAO(),
	_program(std::make_unique<QOpenGLShaderProgram>())
{
	_vertexData.resize(20);
}

void QuadRenderer::init()
{
	initializeOpenGLFunctions();
	initializePrograms();

	_quadVBO.create();
	_quadVBO.bind();
	_quadVBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_quadVBO.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_quadVBO.release();

	_quadVAO.create();

	_program->bind();
	_quadVAO.bind();
	_quadVBO.bind();

	const auto stride = 5 * sizeof(GLfloat);

	_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
	_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
	_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, stride);
	_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

	_quadVAO.release();
	_quadVBO.release();
	_program->release();
}

void QuadRenderer::resize(QSize renderSize)
{
}

void QuadRenderer::render()
{
	if (!initialized())
		return;
	
	_quadVAO.bind();
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	_quadVAO.release();
}

void QuadRenderer::destroy()
{
	_quadVBO.destroy();
	_quadVAO.destroy();
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

	_quadVAO.bind();
	{
		_quadVBO.bind();
		{
			_quadVBO.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		}
		_quadVBO.release();
	}
	_quadVAO.release();
}