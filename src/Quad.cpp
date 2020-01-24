#include "Quad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

std::uint32_t Quad::_vertexAttribute	= 0;
std::uint32_t Quad::_textureAttribute	= 1;

Quad::Quad(const QString& name /*= "Quad"*/) :
	Shape(name),
	_rectangle(),
	_vertexData()
{
	_vertexData.resize(20);
}

void Quad::initialize()
{
	Shape::initialize();

	if (!_shaderPrograms.contains("Quad") || !_vaos.contains("Quad") || !_vbos.contains("Quad"))
		return;

	vbo("Quad")->bind();
	{
		vbo("Quad")->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vbo("Quad")->allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		vbo("Quad")->release();
	}

	if (shaderProgram("Quad")->isLinked() && shaderProgram("Quad")->bind()) {
		const auto stride = 5 * sizeof(GLfloat);

		vao("Quad")->bind();
		vbo("Quad")->bind();

		shaderProgram("Quad")->enableAttributeArray(Quad::_vertexAttribute);
		shaderProgram("Quad")->enableAttributeArray(Quad::_textureAttribute);
		shaderProgram("Quad")->setAttributeBuffer(Quad::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		shaderProgram("Quad")->setAttributeBuffer(Quad::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		vao("Quad")->release();
		vbo("Quad")->release();

		shaderProgram("Quad")->release();

		_initialized = true;
	}
}

void Quad::render()
{
	if (!canRender())
		return;

	Shape::render();
	
	//qDebug() << "Render" << _name << "shape";

	if (isTextured()) {
		texture("Quad")->bind();
	}

	if (bindShaderProgram("Quad")) {
		vao("Quad")->bind();
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		vao("Quad")->release();

		shaderProgram("Quad")->release();
	}

	if (isTextured()) {
		texture("Quad")->release();
	}
}

QRectF Quad::rectangle() const
{
	return _rectangle;
}

QSizeF Quad::size() const
{
	return _rectangle.size();
}

void Quad::setRectangle(const QRectF& rectangle)
{
	if (rectangle == _rectangle)
		return;
	
	qDebug() << "Set quad rectangle";

	_rectangle = rectangle;

	emit rectangleChanged(_rectangle);

	createQuad();
}

bool Quad::isTextured() const
{
	return texture("Quad").get() != nullptr && texture("Quad")->isCreated();
}

void Quad::addVAOs()
{
	qDebug() << "Add OpenGL VAO's to" << _name << "shape";

	addVAO("Quad", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao("Quad")->create();
}

void Quad::addVBOs()
{
	qDebug() << "Add OpenGL VBO's to" << _name << "shape";

	addVBO("Quad", QSharedPointer<QOpenGLBuffer>::create());

	vbo("Quad")->create();
}

bool Quad::canRender() const
{
	return Shape::canRender() && _rectangle.isValid();
}

void Quad::createQuad()
{
	const float width	= _rectangle.width();
	const float height	= _rectangle.height();

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

	vbo("Quad")->bind();
	{
		vbo("Quad")->allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		vbo("Quad")->release();
	}
}