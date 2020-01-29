#include "Polyline2D.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"
#include "Renderer.h"

std::uint32_t Polyline2D::_vertexAttribute = 0;

Polyline2D::Polyline2D(Renderer* renderer, const QString& name, const float& z /*= 0.f*/, const bool& closed /*= true*/, const float& lineWidth /*= 1.f*/, const float& textureScale /*= 0.05f*/) :
	Shape(renderer, name),
	_closed(closed),
	_lineWidth(lineWidth),
	_textureScale(textureScale),
	_noPoints(0)
{
	setTranslation(QVector3D(0.f, 0.f, z));
}

void Polyline2D::initialize()
{
	Shape::initialize();

	auto polylineShaderProgram	= shaderProgram("Polyline");
	auto polylineVAO			= vao("Polyline");
	auto polylineVBO			= vbo("Polyline");

	if (polylineShaderProgram->isLinked() && polylineShaderProgram->bind()) {
		polylineVAO->bind();
		polylineVBO->bind();

		const auto stride = 3 * sizeof(GLfloat);

		polylineShaderProgram->enableAttributeArray(Polyline2D::_vertexAttribute);
		polylineShaderProgram->setAttributeBuffer(Polyline2D::_vertexAttribute, GL_FLOAT, 0, 3);

		polylineVBO->release();
		polylineVAO->release();

		polylineShaderProgram->release();

		_initialized = true;
	}
}

void Polyline2D::render()
{
	if (!canRender())
		return;

	Shape::render();

	//qDebug() << "Render" << _name << "shape";

	if (isTextured()) {
		texture("Polyline")->bind();
	}

	if (bindShaderProgram("Polyline")) {
		vao("Polyline")->bind();
		{
			glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, _noPoints);
		}
		vao("Polyline")->release();

		shaderProgram("Polyline")->release();
	}

	if (isTextured()) {
		texture("Polyline")->release();
	}
}

float Polyline2D::lineWidth() const
{
	return _lineWidth;
}

void Polyline2D::setLineWidth(const float& lineWidth)
{
	if (lineWidth == _lineWidth)
		return;

	qDebug() << "Set polyline line width to" << QString::number(lineWidth, 'f', 1);

	_lineWidth = lineWidth;

	emit lineWidthChanged(_lineWidth);
}

void Polyline2D::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Vertex, polyline2DVertexShaderSource.c_str());
	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Geometry, polyline2DGeometryShaderSource.c_str());
	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Fragment, polyline2DFragmentShaderSource.c_str());
	shaderProgram("Polyline")->link();
}

void Polyline2D::addVAOs()
{
	qDebug() << "Add OpenGL VAO's to" << _name << "shape";

	addVAO("Polyline", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao("Polyline")->create();
}

void Polyline2D::addVBOs()
{
	qDebug() << "Add OpenGL VBO's to" << _name << "shape";

	addVBO("Polyline", QSharedPointer<QOpenGLBuffer>::create());

	vbo("Polyline")->create();
}

void Polyline2D::setPoints(QVector<QVector2D> points)
{
	//qDebug() << "Set polyline points";

	_noPoints = points.size();

	QVector<float> vertexData;

	vertexData.reserve(points.size() * 3);
	
	for (auto point : points) {
		vertexData.append(point.x());
		vertexData.append(point.y());
		vertexData.append(0.f);
	}

	vbo("Polyline")->bind();
	{
		vbo("Polyline")->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vbo("Polyline")->allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
		vbo("Polyline")->release();
	}
}

void Polyline2D::configureShaderProgram(const QString& name)
{
	//qDebug() << "Configuring shader program" << name << "for" << _name;

	if (name == "Polyline") {
		shaderProgram("Polyline")->setUniformValue("lineTexture", 0);
		shaderProgram("Polyline")->setUniformValue("transform", modelViewProjectionMatrix());
		shaderProgram("Polyline")->setUniformValue("lineWidth", _lineWidth);
	}
}

bool Polyline2D::isTextured() const
{
	return texture("Polyline").get() != nullptr && texture("Polyline")->isCreated();
}