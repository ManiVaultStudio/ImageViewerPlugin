#include "Bounds.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

Bounds::Bounds(const QString& name /*= "Bounds"*/) :
	Polyline2D(name),
	_bounds()
{
}

void Bounds::setBounds(const QRectF& bounds)
{
	if (bounds == _bounds)
		return;

	_bounds = bounds;
	
	/*
	_bounds = selectionBounds;

	const auto p0 = _bounds.topLeft();
	const auto p1 = _bounds.bottomRight();

	QVector<QVector2D> points;

	points.append(QVector2D(p0.x(), p0.y()));
	points.append(QVector2D(p1.x(), p0.y()));
	points.append(QVector2D(p1.x(), p1.y()));
	points.append(QVector2D(p0.x(), p1.y()));

	auto boundsStippleTexture = texture("BoundsStipple");

	boundsStippleTexture->bind();
	{
		_boundsPolyline.setPoints(points);
	}
	*/
}

void Bounds::addShaderPrograms()
{
	qDebug() << "Adding shader programs";

	addShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram()->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	shaderProgram()->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	shaderProgram()->link();
}

void Bounds::addVAOs()
{
	qDebug() << "Adding vertex array objects";

	addVAO("Polyline", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao()->create();
}

void Bounds::addVBOs()
{
	qDebug() << "Adding vertex buffer objects";

	addVBO("Polyline", QSharedPointer<QOpenGLBuffer>::create());

	vbo()->create();
}

void Bounds::addTextures()
{
	qDebug() << "Adding textures";

	addTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));
}