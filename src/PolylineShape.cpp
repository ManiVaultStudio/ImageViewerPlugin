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

void PolylineShape::setPoints(QVector<Point> points /*= QVector<PolylinePoint2D>()*/)
{
	//qDebug() << "Set polyline points";

	_points = points;

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			_vbo.allocate(_points.constData(), _points.count() * sizeof(Point));
		}
		_vbo.release();
	}
	_vao.release();
}

void PolylineShape::render()
{
	if (_points.size() <= 1)
		return;

	Shape::render();

	qDebug() << "Render" << _name << "shape";

	_vao.bind();
	{
		glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, _points.size());
	}
	_vao.release();
}