#include "SelectionBounds.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

SelectionBounds::SelectionBounds(const QString& name /*= "Bounds"*/, const QColor& color /*= QColor(255, 160, 70, 150)*/) :
	Polyline2D(name),
	_bounds(),
	_color(color)
{
}

void SelectionBounds::setBounds(const QRectF& bounds)
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

	emit boundsChanged(_bounds);
}

void SelectionBounds::setColor(const QColor& color)
{
	if (color == _color)
		return;

	_color = color;

	emit colorChanged(_color);
}

void SelectionBounds::addShaderPrograms()
{
	qDebug() << "Adding shader programs";

	addShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram()->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	shaderProgram()->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	shaderProgram()->link();
}

void SelectionBounds::addVAOs()
{
	qDebug() << "Adding vertex array objects";

	addVAO("Polyline", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao()->create();
}

void SelectionBounds::addVBOs()
{
	qDebug() << "Adding vertex buffer objects";

	addVBO("Polyline", QSharedPointer<QOpenGLBuffer>::create());

	vbo()->create();
}

void SelectionBounds::addTextures()
{
	qDebug() << "Adding textures";

	addTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _color);

	texture()->setWrapMode(QOpenGLTexture::Repeat);
	texture()->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}