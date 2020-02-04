#include "SelectionRectangleProp.h"
#include "Actor.h"
#include "Renderer.h"
#include "PolylineShape.h"

#include <QDebug>

SelectionRectangleProp::SelectionRectangleProp(Actor* actor, const QString& name) :
	PolylineProp(actor, name),
	_rectangle()
{
}

QRect SelectionRectangleProp::rectangle()
{
	return _rectangle;
}

void SelectionRectangleProp::setRectangle(const QRect& rectangle)
{
	const auto normalizedRectangle = rectangle.normalized();

	if (normalizedRectangle == _rectangle)
		return;

	_rectangle = normalizedRectangle;

	if (_rectangle.isNull()) {
		setPoints(QVector<QVector3D>());
	}

	QVector<QVector3D> points;

	const auto start = renderer()->screenToWorld(actor()->modelViewMatrix(), _rectangle.topLeft());
	const auto end = renderer()->screenToWorld(actor()->modelViewMatrix(), _rectangle.bottomRight());

	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));

	setPoints(points);
}