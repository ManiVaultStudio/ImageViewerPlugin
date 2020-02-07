#include "DiskShape.h"
#include "Prop.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QtMath>
#include <QDebug>

DiskShape::DiskShape(Prop* prop, const QString& name) :
	Shape(prop, name),
	_vertices()
{
}

bool DiskShape::canRender() const
{
	return _vertices.size() > 1;
}

void DiskShape::render()
{
	if (!canRender())
		return;

	//qDebug() << "Render" << _name << "shape";

	_vao.bind();
	{
		glDrawArrays(GL_TRIANGLE_FAN, 0, _vertices.size());
	}
	_vao.release();
}

void DiskShape::set(const QVector3D& center, const float& radius)
{
	const auto noSegments = 32;

	_vertices.clear();
	_vertices.resize(noSegments * 3);

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta = 2.0f * M_PI * float(s) / static_cast<float>(noSegments);
		_vertices.append(center + QVector2D(radius * cosf(theta), radius * sinf(theta)));
	}

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			_vbo.allocate(_vertices.constData(), _vertices.count() * sizeof(QVector3D));
		}
		_vbo.release();
	}
	_vao.release();
}