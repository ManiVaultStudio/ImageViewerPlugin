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
	

	/*
	auto quadVAO = vao("Quad");
	auto quadVBO = vbo("Quad");

	auto quadShaderProgram = shaderProgram("Quad");

	if (quadShaderProgram->isLinked() && quadShaderProgram->bind()) {
		const auto stride = 5 * sizeof(GLfloat);

		quadVAO->bind();
		quadVBO->bind();

		quadShaderProgram->enableAttributeArray(Quad::_vertexAttribute);
		quadShaderProgram->enableAttributeArray(Quad::_textureAttribute);
		quadShaderProgram->setAttributeBuffer(Quad::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		quadShaderProgram->setAttributeBuffer(Quad::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVAO->release();
		quadVBO->release();

		quadShaderProgram->release();

		_initialized = true;
	}
	*/
}

void QuadShape::render()
{
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

	//qDebug() << "Set quad rectangle" << _rectangle;
	
	emit rectangleChanged(_rectangle);

	createQuad();
}

void QuadShape::createQuad()
{
	const float width	= _rectangle.width();
	const float height	= _rectangle.height();

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