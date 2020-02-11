#include "RectangleSelectionActor.h"
#include "PolylineProp.h"
#include "Renderer.h"

#include <QKeyEvent>
#include <QMenu>
#include <QColor>
#include <QOpenGLTexture>
#include <QGuiApplication>
#include <QVector3D>
#include <QDebug>

#include <QtMath>

RectangleSelectionActor::RectangleSelectionActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	/*
	_registeredEvents |= static_cast<int>(ActorEvent::MousePress);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseRelease);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseMove);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseWheel);
	

	addProp<PolylineProp>("RectangleProp");
	addProp<PolylineProp>("BrushProp");
	addProp<PointsProp>("BrushCenter");
	addProp<PolylineProp>("LassoSegmentsProp");
	addProp<PolylineProp>("LassoClosingSegmentProp");
	addProp<PolylineProp>("PolygonSegmentsProp");
	addProp<PolylineProp>("PolygonClosingSegmentProp");
	addProp<PointsProp>("PolygonPointsProp");
	addProp<InterimSelectionProp>("InterimSelectionProp");
	*/
}

void RectangleSelectionActor::initialize()
{
	Actor::initialize();

	/*
	// Configure rectangle selection prop
	rectangleProp()->setLineWidth(2.5f);
	rectangleProp()->setLineColor(renderer()->colorByName("SelectionOutline", 200));

	// Configure brush selection prop
	brushProp()->setLineWidth(2.5f);

	// Configure lasso selection props
	lassoSegmentsProp()->setClosed(false);
	lassoSegmentsProp()->setLineWidth(3.5f);
	lassoSegmentsProp()->setLineColor(renderer()->colorByName("SelectionOutline", 200));
	lassoClosingSegmentProp()->setClosed(false);
	lassoClosingSegmentProp()->setLineWidth(2.0f);
	lassoClosingSegmentProp()->setLineColor(renderer()->colorByName("SelectionOutline", 100));

	// Configure polygon selection props
	polygonSegmentsProp()->setClosed(false);
	polygonSegmentsProp()->setLineWidth(2.5f);
	polygonSegmentsProp()->setLineColor(renderer()->colorByName("SelectionOutline", 200));
	polygonClosingSegmentProp()->setClosed(false);
	polygonClosingSegmentProp()->setLineWidth(1.5f);
	polygonClosingSegmentProp()->setLineColor(renderer()->colorByName("SelectionOutline", 100));
	*/
}

/*
void RectangleSelectionActor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMousePressEvent())
		return;

	Actor::onMousePressEvent(mouseEvent);

	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				startSelection();
				addMouseEvent(mouseEvent);
				update();
				break;
			}
		}

		case SelectionType::Brush:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				startSelection();
				addMouseEvent(mouseEvent);
				update();
				break;
			}
		}

		case SelectionType::Lasso:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				startSelection();
			}
			break;
		}

		case SelectionType::Polygon:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				if (!_selecting)
					startSelection();

				addMouseEvent(mouseEvent);
				update();
			}
			break;
		}

		default:
			break;
	}
}

void RectangleSelectionActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseReleaseEvent())
		return;

	Actor::onMouseReleaseEvent(mouseEvent);

	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
			}
			break;
		}

		
		case SelectionType::Brush:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
			}
			break;
		}
		
		case SelectionType::Lasso:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
				update();
			}
			break;
		}

		case SelectionType::Polygon:
		{
			if (mouseEvent->button() == Qt::RightButton) {
				endSelection();
				update();
			}
			break;
		}

		default:
			break;
	}
}

void RectangleSelectionActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseMoveEvent())
		return;

	Actor::onMouseMoveEvent(mouseEvent);

	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (mouseEvent->buttons() & Qt::LeftButton)
			{
				addMouseEvent(mouseEvent);
				update();
			}
			break;
		}

		
		case SelectionType::Brush:
		{
			addMouseEvent(mouseEvent);
			update();
			break;
		}

		case SelectionType::Polygon:
		{
			if (_mouseEvents.size() == 1)
			{
				addMouseEvent(mouseEvent);
				update();
			}

			if (_mouseEvents.size() >= 2)
			{
				const auto pScreen = QVector2D(mouseEvent->pos());
				_mouseEvents.last() = MouseEvent(pScreen, renderer()->screenPointToWorldPosition(modelViewMatrix(), pScreen));
				update();
			}
			break;
		}

		default:
			break;
	}
}
*/
void RectangleSelectionActor::update()
{
	/*
	QVector<QVector3D> points;

	// No need to draw a rectangle if less than two mouse events were recorded
	if (_mouseEvents.size() >= 2) {
		// Get first and last recorded mouse position in world coordinates
		const auto pWorldA = _mouseEvents.first().worldPosition();
		const auto pWorldB = _mouseEvents.last().worldPosition();

		// Create a normalized rectangle
		auto rectangle = QRectF(QPointF(pWorldA.x(), pWorldA.y()), QPointF(pWorldB.x(), pWorldB.y())).normalized();

		// Compute rectangle start and end in screen coordinates
		const auto topLeft = rectangle.topLeft();
		const auto bottomRight = rectangle.bottomRight();
		const auto start = renderer()->worldPositionToScreenPoint(QVector3D(topLeft.x(), topLeft.y(), 0.f));
		const auto end = renderer()->worldPositionToScreenPoint(QVector3D(bottomRight.x(), bottomRight.y(), 0.f));

		// Create polyline points
		points.append(QVector3D(start.x(), start.y(), 0.f));
		points.append(QVector3D(end.x(), start.y(), 0.f));
		points.append(QVector3D(end.x(), end.y(), 0.f));
		points.append(QVector3D(start.x(), end.y(), 0.f));
	}

	rectangleProp()->setPoints(points);
	*/
}

PolylineProp* RectangleSelectionActor::rectangleProp()
{
	return propByName<PolylineProp>("RectangleProp");
}