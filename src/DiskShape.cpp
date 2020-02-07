#include "DiskShape.h"
#include "Prop.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QtMath>
#include <QDebug>

DiskShape::DiskShape(Prop* prop, const QString& name) :
	Shape(prop, name),
	_points()
{
}

bool DiskShape::canRender() const
{
	return _points.size() >= 1;
}

void DiskShape::render()
{
	if (!canRender())
		return;

	//qDebug() << "Render" << _name << "shape";
	qDebug() << "==========";

	_vao.bind();
	{
		glDrawArrays(GL_POINTS, 0, _points.size());
	}
	_vao.release();
}

void DiskShape::setPoints(const QVector<QVector3D>& centers)
{
	/*
	const auto noSegments = 32;

	const float radius = 1.0f;

	_points.clear();
	_points.resize(noSegments * 3);

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta = 2.0f * M_PI * float(s) / static_cast<float>(noSegments);
		_points.append(center + QVector2D(radius * cosf(theta), radius * sinf(theta)));
	}
	*/

	_points = centers;

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			_vbo.allocate(centers.constData(), centers.count() * sizeof(QVector3D));
		}
		_vbo.release();
	}
	_vao.release();
}