#include "SelectionBounds.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

SelectionBounds::SelectionBounds(Actor* actor, const QString& name, const float& z /*= 0.f*/, const QColor& color /*= QColor(255, 153, 0, 150)*/) :
	Polyline2D(actor, name, z, true, 0.005f),
	_bounds(),
	_color(color)
{
}

void SelectionBounds::setBounds(const QRectF& bounds)
{
	if (bounds == _bounds)
		return;

	qDebug() << "Set selection bounds shape bounds";

	_bounds = bounds;
	
	const auto p0 = _bounds.topLeft();
	const auto p1 = _bounds.bottomRight() - QPointF(1.f, 1.f);

	QVector<PolylinePoint2D> points;

	points.append(PolylinePoint2D(QVector3D(p0.x(), p0.y(), 0.f), QVector2D(), _lineWidth));
	points.append(PolylinePoint2D(QVector3D(p0.x(), p0.y(), 0.f), QVector2D(), _lineWidth));
	points.append(PolylinePoint2D(QVector3D(p1.x(), p0.y(), 0.f), QVector2D(), _lineWidth));
	points.append(PolylinePoint2D(QVector3D(p1.x(), p1.y(), 0.f), QVector2D(), _lineWidth));
	points.append(PolylinePoint2D(QVector3D(p0.x(), p1.y(), 0.f), QVector2D(), _lineWidth));
	points.append(PolylinePoint2D(QVector3D(p0.x(), p1.y(), 0.f), QVector2D(), _lineWidth));

	setPoints(points);

	emit boundsChanged(_bounds);
}

void SelectionBounds::setColor(const QColor& color)
{
	if (color == _color)
		return;

	qDebug() << "Set selection bounds shape color";

	_color = color;

	emit colorChanged(_color);
}

void SelectionBounds::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _color);

	addTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(textureImage));

	texture("Polyline")->setWrapMode(QOpenGLTexture::Repeat);
	texture("Polyline")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void SelectionBounds::configureShaderProgram(const QString& name)
{
	Polyline2D::configureShaderProgram(name);
}