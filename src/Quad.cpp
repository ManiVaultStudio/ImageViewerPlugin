#include "Quad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

std::uint32_t Quad::_vertexAttribute	= 0;
std::uint32_t Quad::_textureAttribute	= 1;

Quad::Quad(Renderer* renderer, const QString& name, const float& z /*= 0.f*/) :
	Shape(renderer, name),
	_rectangle(),
	_vertexData()
{
	_vertexData.resize(20);

	setTranslation(QVector3D(0.f, 0.f, z));
}

void Quad::initialize()
{
	Shape::initialize();

	if (!_shaderPrograms.contains("Quad") || !_vaos.contains("Quad") || !_vbos.contains("Quad"))
		return;

	auto quadVAO = vao("Quad");
	auto quadVBO = vbo("Quad");

	quadVBO->bind();
	{
		quadVBO->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		quadVBO->allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		quadVBO->release();
	}

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

	_rectangle = rectangle;

	//qDebug() << "Set quad rectangle" << _rectangle;

	setTranslation(QVector3D(-0.5f * rectangle.width(), -0.5f * rectangle.bottom(), _modelMatrix.column(3).z()));

	emit rectangleChanged(_rectangle);
	emit sizeChanged(_rectangle.size());

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

void Quad::configureShaderProgram(const QString& name)
{
	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("transform", modelViewProjectionMatrix());
	}
}

void Quad::createQuad()
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

	vbo("Quad")->bind();
	{
		vbo("Quad")->allocate(_vertexData.constData(), _vertexData.count() * sizeof(GLfloat));
		vbo("Quad")->release();
	}
}