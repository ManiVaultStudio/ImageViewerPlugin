#include "QuadShape.h"
#include "Prop.h"

#include <QDebug>

std::uint32_t QuadShape::_vertexAttribute	= 0;
std::uint32_t QuadShape::_textureAttribute	= 1;

QuadShape::QuadShape(Prop* prop, const QString& name) :
	Shape(prop, name),
	_rectangle(),
	_vertexData()
{
	_vertexData.resize(20);
}

void QuadShape::initialize()
{
	Shape::initialize();

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		}
		_vbo.release();
	}
}

bool QuadShape::canRender() const
{
	return !_rectangle.isNull();
}

void QuadShape::render()
{
	if (!canRender())
		return;

	Shape::render();

	_vao.bind();
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
	_vao.release();
}

QRectF QuadShape::rectangle() const
{
	return _rectangle;
}

QSizeF QuadShape::imageSize() const
{
	return _rectangle.size();
}

void QuadShape::setRectangle(const QRectF& rectangle)
{
	if (rectangle == _rectangle)
		return;

	_rectangle = rectangle;

	createQuad();
}

void QuadShape::createQuad()
{
	const float coordinates[4][3] = {
		{ _rectangle.left(),		_rectangle.top(),		0.0f },
		{ _rectangle.right(),		_rectangle.top(),		0.0f },
		{ _rectangle.right(),		_rectangle.bottom(),	0.0f },
		{ _rectangle.left(),		_rectangle.bottom(),	0.0f }
	};

	for (int j = 0; j < 4; ++j)
	{
		_vertexData[j * 5 + 0] = coordinates[j][0];
		_vertexData[j * 5 + 1] = coordinates[j][1];
		_vertexData[j * 5 + 2] = coordinates[j][2];

		_vertexData[j * 5 + 3] = j == 0 || j == 3;
		_vertexData[j * 5 + 4] = j == 2 || j == 3;
	}

	_vbo.bind();
	{
		_vbo.allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		_vbo.release();
	}
}