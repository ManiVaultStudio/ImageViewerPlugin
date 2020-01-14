#include "QuadRenderer.h"

#include <QDebug>

#include <vector>

#include "Shaders.h"

std::uint32_t QuadRenderer::_quadVertexAttribute = 0;
std::uint32_t QuadRenderer::_quadTextureAttribute = 1;

QuadRenderer::QuadRenderer(const std::uint32_t& zIndex) :
	StackedRenderer(zIndex),
	_size(),
	_vertexData(),
	_quadVBO(),
	_quadVAO()
{
	_vertexData.resize(20);
}

void QuadRenderer::init()
{
	StackedRenderer::init();

	_quadVBO.create();
	_quadVBO.bind();
	_quadVBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_quadVBO.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
	_quadVBO.release();

	_quadVAO.create();
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