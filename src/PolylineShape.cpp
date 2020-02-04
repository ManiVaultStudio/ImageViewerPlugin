#include "PolylineShape.h"
#include "Prop.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

PolylineShape::Point::Point() :
	_position(),
	_textureCoordinate(),
	_lineWidth()
{
}

PolylineShape::Point::Point(const QVector3D& position, const QVector2D& textureCoordinate, const float& lineWidth) :
	_position(position),
	_textureCoordinate(textureCoordinate),
	_lineWidth(lineWidth)
{
}

std::uint32_t PolylineShape::Point::_positionAttribute				= 0;
std::uint32_t PolylineShape::Point::_textureCoordinateAttribute		= 1;
std::uint32_t PolylineShape::Point::_lineWidthAttribute				= 2;

PolylineShape::PolylineShape(Prop* prop, const QString& name) :
	Shape(prop, name),
	_closed(true),
	_textureScale(1.0f),
	_points()
{
}

void PolylineShape::initialize()
{
	Shape::initialize();

	/*
	auto polylineShaderProgram	= shaderProgram("Polyline");
	auto polylineVAO			= vao("Polyline");
	auto polylineVBO			= vbo("Polyline");

	if (polylineShaderProgram->isLinked() && polylineShaderProgram->bind()) {
		polylineVAO->bind();
		polylineVBO->bind();

		const auto stride = 6 * sizeof(GLfloat);

		polylineShaderProgram->enableAttributeArray(PolylinePoint2D::_positionAttribute);
		polylineShaderProgram->enableAttributeArray(PolylinePoint2D::_textureCoordinateAttribute);
		polylineShaderProgram->enableAttributeArray(PolylinePoint2D::_lineWidthAttribute);

		polylineShaderProgram->setAttributeBuffer(PolylinePoint2D::_positionAttribute, GL_FLOAT, 0, 3, stride);
		polylineShaderProgram->setAttributeBuffer(PolylinePoint2D::_textureCoordinateAttribute, GL_FLOAT, 3, 3, stride);
		polylineShaderProgram->setAttributeBuffer(PolylinePoint2D::_lineWidthAttribute, GL_FLOAT, 5, 3, stride);

		polylineVBO->release();
		polylineVAO->release();

		polylineShaderProgram->release();

		_initialized = true;
	}
	*/
}

void PolylineShape::setPoints(QVector<Point> points /*= QVector<PolylinePoint2D>()*/)
{
	//qDebug() << "Set polyline points";

	_points = points;

	_vbo.bind();
	{
		_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
		_vbo.allocate(_points.constData(), _points.count() * sizeof(Point));
		_vbo.release();
	}
}

/*
bool Polyline2D::canRender() const
{
	if (!Shape::canRender())
		return false;

	const auto hasPoints		= _points.size();
	const auto validLineWidth	= _lineWidth > 0.f;

	return hasPoints && validLineWidth;
}

void Polyline2D::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	setShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Vertex, polyline2DVertexShaderSource.c_str());
	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Geometry, polyline2DGeometryShaderSource.c_str());
	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Fragment, polyline2DFragmentShaderSource.c_str());
	shaderProgram("Polyline")->link();
}

void Polyline2D::configureShaderProgram(const QString& name)
{
	//qDebug() << "Configuring shader program" << name << "for" << _name;

	if (name == "Polyline") {
		shaderProgram("Polyline")->setUniformValue("lineTexture", 0);
		shaderProgram("Polyline")->setUniformValue("transform", _actor->modelViewProjectionMatrix());
		shaderProgram("Polyline")->setUniformValue("lineWidth", _lineWidth);
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
			glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, _points.size());
		}
		vao("Polyline")->release();

		shaderProgram("Polyline")->release();
	}

	if (isTextured()) {
		texture("Polyline")->release();
	}
}
*/