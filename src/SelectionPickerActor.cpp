#include "SelectionPickerActor.h"
#include "PolylineProp.h"
#include "SelectionBrushProp.h"
#include "SelectionLassoProp.h"
#include "SelectionPolygonProp.h"
#include "Renderer.h"

#include <QKeyEvent>
#include <QMenu>
#include <QColor>
#include <QOpenGLTexture>
#include <QGuiApplication>
#include <QVector3D>
#include <QDebug>

#include <QtMath>

SelectionPickerActor::SelectionPickerActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageSize(),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(100.0f),
	_brushRadiusDelta(10.f),
	_mouseEvents(),
	_outlineLineWidth(0.0025f)
{
	_registeredEvents |= static_cast<int>(ActorEvent::MousePress);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseRelease);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseMove);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseWheel);
	_registeredEvents |= static_cast<int>(ActorEvent::KeyPress);
	_registeredEvents |= static_cast<int>(ActorEvent::KeyRelease);

	addProp<PolylineProp>("SelectionRectangleProp");
	addProp<PolylineProp>("SelectionBrushProp");
	addProp<PolylineProp>("SelectionLassoProp");
	addProp<PolylineProp>("PolygonSegmentsProp");
	addProp<PolylineProp>("PolygonClosingSegmentProp");
}

void SelectionPickerActor::show()
{
	Actor::show();

	_mouseEvents.clear();

	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}
}

void SelectionPickerActor::hide()
{
	Actor::hide();

	_mouseEvents.clear();
}

void SelectionPickerActor::initialize()
{
	Actor::initialize();

	rectangleProp()->setLineWidth(2);
	selectionBrushProp()->setLineWidth(2);

	polygonSegmentsProp()->setClosed(false);
	polygonSegmentsProp()->setLineWidth(2.0f);
	polygonSegmentsProp()->setLineColor(QColor(255, 165, 0, 200));
	
	polygonClosingSegmentProp()->setClosed(false);
	polygonClosingSegmentProp()->setLineWidth(1.5f);
	polygonClosingSegmentProp()->setLineColor(QColor(255, 165, 0, 100));
}

QSize SelectionPickerActor::imageSize() const
{
	return _imageSize;
}

void SelectionPickerActor::setImageSize(const QSize& imageSize)
{
	if (imageSize == _imageSize)
		return;

	qDebug() << "Set image size";

	_imageSize = imageSize;

	emit imageSizeChanged(_imageSize);
}

SelectionType SelectionPickerActor::selectionType() const
{
	return _selectionType;
}

void SelectionPickerActor::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;
	
	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}

	_selectionType = selectionType;

	qDebug() << "Set selection type to" << selectionTypeName(_selectionType);

	_mouseEvents.clear();

	startSelection();

	emit selectionTypeChanged(_selectionType);
}

SelectionModifier SelectionPickerActor::selectionModifier() const
{
	return _selectionModifier;
}

void SelectionPickerActor::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	_selectionModifier = selectionModifier;

	qDebug() << "Set selection modifier to" << selectionModifierName(selectionModifier);

	emit selectionModifierChanged(_selectionModifier);
}

float SelectionPickerActor::brushRadius() const
{
	return _brushRadius;
}

void SelectionPickerActor::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 1000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius to" << QString::number(_brushRadius, 'f', 1);

	if (_selectionType == SelectionType::Brush)
		updateSelectionBrush();
}

float SelectionPickerActor::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectionPickerActor::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.1f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = boundBrushRadiusDelta;

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;

	emit brushRadiusDeltaChanged(_brushRadiusDelta);
}

void SelectionPickerActor::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectionPickerActor::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

void SelectionPickerActor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton) {
		addMouseEvent(mouseEvent);
		startSelection();
	}
}

void SelectionPickerActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
				updateSelectionRectangle();
			}
			break;
		}

		case SelectionType::Brush:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				updateSelectionBrush();
				endSelection();
			}
			break;
		}

		case SelectionType::Lasso:
		{
			/*
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
				lassoShape()->reset();
			}
			*/
			break;
		}

		case SelectionType::Polygon:
		{
			if (mouseEvent->button() == Qt::RightButton) {
				endSelection();
				updateSelectionPolygon();
			}
			break;
		}

		default:
			break;
	}
}

void SelectionPickerActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			if (mouseEvent->buttons() & Qt::LeftButton)
			{
				addMouseEvent(mouseEvent);
				updateSelectionRectangle();
			}
			break;
		}

		case SelectionType::Brush:
		{
			addMouseEvent(mouseEvent);
			updateSelectionBrush();
			break;
		}

		case SelectionType::Lasso:
		{
			/*
			if (mouseEvent->buttons() & Qt::LeftButton)
			{
				addMousePosition(mouseEvent->pos());

				if (_mousePositions.size() >= 2)
					updateLasso();
			}
			*/
			break;
		}

		case SelectionType::Polygon:
		{
			if (_mouseEvents.size() == 1)
			{
				addMouseEvent(mouseEvent);
				updateSelectionPolygon();
			}

			if (_mouseEvents.size() >= 2)
			{
				_mouseEvents.last() = MouseEvent(mouseEvent->pos(), renderer()->screenPointToWorldPosition(modelViewMatrix(), mouseEvent->pos()));
				updateSelectionPolygon();
			}
			break;
		}

		default:
			break;
	}
}

void SelectionPickerActor::onMouseWheelEvent(QWheelEvent* wheelEvent)
{
	switch (_selectionType)
	{
		case SelectionType::None:
			break;
		case SelectionType::Rectangle:
			break;
		case SelectionType::Brush:
		{
			if (wheelEvent->delta() > 0) {
				brushSizeIncrease();
			}
			else {
				brushSizeDecrease();
			}
			break;
		}
		case SelectionType::Lasso:
			break;
		case SelectionType::Polygon:
			break;
		default:
			break;
	}
}

void SelectionPickerActor::onKeyPressEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
		case Qt::Key::Key_R:
			setSelectionType(SelectionType::Rectangle);
			break;

		case Qt::Key::Key_B:
			setSelectionType(SelectionType::Brush);
			break;

		case Qt::Key::Key_L:
			setSelectionType(SelectionType::Lasso);
			break;

		case Qt::Key::Key_P:
			setSelectionType(SelectionType::Polygon);
			break;

		case Qt::Key::Key_Shift:
			setSelectionModifier(SelectionModifier::Add);
			break;

		case Qt::Key::Key_Control:
			setSelectionModifier(SelectionModifier::Remove);
			break;

		case Qt::Key::Key_Escape:
			endSelection();
			break;

		default:
			break;
	}
}

void SelectionPickerActor::onKeyReleaseEvent(QKeyEvent* keyEvent)
{
	switch (keyEvent->key())
	{
		case Qt::Key::Key_Shift:
		case Qt::Key::Key_Control:
		{
			setSelectionModifier(SelectionModifier::Replace);
			break;
		}

		default:
			break;
	}
}

void SelectionPickerActor::startSelection()
{
	qDebug() << "Start selection" << _name;

	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			rectangleProp()->show();
			updateSelectionRectangle();
			break;

		case SelectionType::Brush:
			selectionBrushProp()->show();
			updateSelectionBrush();
			break;

		case SelectionType::Lasso:
			//lassoShape()->show();
			break;

		case SelectionType::Polygon:
			polygonSegmentsProp()->show();
			polygonClosingSegmentProp()->show();
			break;

		default:
			break;
	}
}

void SelectionPickerActor::endSelection()
{
	qDebug() << "End selection" << _name;

	_mouseEvents.clear();
	
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			updateSelectionRectangle();
			break;

		case SelectionType::Brush:
			updateSelectionBrush();
			break;

		case SelectionType::Lasso:
			updateSelectionBrush();
			break;

		case SelectionType::Polygon:
			updateSelectionPolygon();
			break;

		default:
			break;
	}

	/*
	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}
	*/
}

void SelectionPickerActor::addMouseEvent(QMouseEvent* mouseEvent)
{
	const auto screenPoint		= mouseEvent->pos();
	const auto worldPosition	= _renderer->screenPointToWorldPosition(modelViewMatrix(), screenPoint);

	_mouseEvents.append(MouseEvent(screenPoint, worldPosition));
}

void SelectionPickerActor::updateSelectionRectangle()
{
	QVector<QVector3D> points;

	if (_mouseEvents.size() < 2)
	{
		rectangleProp()->setPoints(points);
		emit changed(this);
		return;
	}

	// Get first and last recorded mouse position in world coordinates
	const auto pWorldA	= _mouseEvents.first().worldPosition();
	const auto pWorldB	= _mouseEvents.last().worldPosition();

	// Create a normalized rectangle
	auto rectangle = QRectF(QPointF(pWorldA.x(), pWorldA.y()), QPointF(pWorldB.x(), pWorldB.y())).normalized();

	// Compute rectangle start and end in screen coordinates
	const auto topLeft		= rectangle.topLeft();
	const auto bottomRight	= rectangle.bottomRight();
	const auto start		= renderer()->worldPositionToScreenPoint(QVector3D(topLeft.x(), topLeft.y(), 0.f));
	const auto end			= renderer()->worldPositionToScreenPoint(QVector3D(bottomRight.x(), bottomRight.y(), 0.f));

	// Create polyline points
	//points.append(QVector3D(start.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), end.y(), 0.f));
	//points.append(QVector3D(start.x(), start.y(), 0.f));
	//points.append(QVector3D(end.x(), start.y(), 0.f));

	//points.insert(0, points[points.size() - 1]);
	//points.append(points[0]);
	//points.append(points[1]);

	rectangleProp()->setPoints(points);

	emit changed(this);
}

void SelectionPickerActor::updateSelectionBrush()
{
	if (_mouseEvents.isEmpty())
		return;

	const auto lastMouseScreenPoint = _mouseEvents.last().screenPoint();

	QVector<QVector3D> points;

	const auto pCenter		= QVector2D(lastMouseScreenPoint.x(), renderer()->viewSize().height() - lastMouseScreenPoint.y());
	const auto noSegments	= 128u;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
		const auto pBrush	= QVector2D(_brushRadius * cosf(theta), _brushRadius * sinf(theta));

		points.append(pCenter + pBrush);
	}

	points.insert(0, points.back());

	const auto leftButtonDown = QGuiApplication::mouseButtons() & Qt::LeftButton;

	selectionBrushProp()->setLineColor(leftButtonDown ? QColor(255, 165, 0, 150) : QColor(255, 165, 0, 50));
	selectionBrushProp()->setLineWidth(leftButtonDown ? 4.0f : 2.0f);
	selectionBrushProp()->setPoints(points);

	emit changed(this);
}

void SelectionPickerActor::updateSelectionLasso()
{
	emit changed(this);
}

void SelectionPickerActor::updateSelectionPolygon()
{
	const auto noMouseEvents = _mouseEvents.size();

	QVector<QVector3D> segmentsPoints, closingSegmentPoints;

	if (noMouseEvents >= 2) {
		for (auto mouseEvent : _mouseEvents) {
			segmentsPoints.append(renderer()->worldPositionToScreenPoint(mouseEvent.worldPosition()));
		}

		segmentsPoints.insert(0, segmentsPoints.first());
		segmentsPoints.append(segmentsPoints.last());
	}
	
	if (noMouseEvents >= 3) {
		const auto pFirst	= _mouseEvents.first().worldPosition();
		const auto pLast	= _mouseEvents.last().worldPosition();

		closingSegmentPoints.append(renderer()->worldPositionToScreenPoint(pFirst));
		closingSegmentPoints.append(renderer()->worldPositionToScreenPoint(pFirst));
		closingSegmentPoints.append(renderer()->worldPositionToScreenPoint(pLast));
		closingSegmentPoints.append(renderer()->worldPositionToScreenPoint(pLast));
	}

	polygonSegmentsProp()->setPoints(segmentsPoints);
	polygonClosingSegmentProp()->setPoints(closingSegmentPoints);

	emit changed(this);
}

PolylineProp* SelectionPickerActor::rectangleProp()
{
	return propByName<PolylineProp>("SelectionRectangleProp");
}

PolylineProp* SelectionPickerActor::selectionBrushProp()
{
	return propByName<PolylineProp>("SelectionBrushProp");
}

PolylineProp* SelectionPickerActor::polygonSegmentsProp()
{
	return propByName<PolylineProp>("PolygonSegmentsProp");
}

PolylineProp* SelectionPickerActor::polygonClosingSegmentProp()
{
	return propByName<PolylineProp>("PolygonClosingSegmentProp");
}

QMenu* SelectionPickerActor::contextMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction = new QAction("Rectangle");
	auto* brushSelectionAction = new QAction("Brush");
	auto* lassoSelectionAction = new QAction("Lasso", this);
	auto* polygonSelectionAction = new QAction("Polygon", this);
	auto* selectNoneAction = new QAction("Select none");
	auto* selectAllAction = new QAction("Select all");
	auto* invertSelectionAction = new QAction("Invert");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle); });

	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush); });
	connect(lassoSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Lasso); });
	connect(polygonSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Polygon); });

	connect(selectAllAction, &QAction::triggered, this, &SelectionPickerActor::selectAll);
	connect(selectNoneAction, &QAction::triggered, this, &SelectionPickerActor::selectNone);
	connect(invertSelectionAction, &QAction::triggered, this, &SelectionPickerActor::invertSelection);

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);
	lassoSelectionAction->setCheckable(true);
	polygonSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_selectionType == SelectionType::Brush);
	lassoSelectionAction->setChecked(_selectionType == SelectionType::Lasso);
	polygonSelectionAction->setChecked(_selectionType == SelectionType::Polygon);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addAction(lassoSelectionAction);
	selectionMenu->addAction(polygonSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(selectAllAction);
	selectionMenu->addAction(selectNoneAction);
	selectionMenu->addAction(invertSelectionAction);

	return selectionMenu;
}

/*


void SelectionPickerActor::updateLasso()
{
	if (_mousePositions.size() == 0)
		return;

	//qDebug() << "Update lasso" << p0 << p1;

	QVector<QVector3D> positions;

	for (auto mousePosition : _mousePositions) {
		const auto pWorld = _renderer->screenToWorld(modelViewMatrix(), mousePosition);
		positions.append(QVector3D(pWorld.x(), pWorld.y(), 0.f));
	}

	QVector<PolylinePoint2D> polylinePoints;

	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _outlineLineWidth));
	}

	polylinePoints.insert(0, polylinePoints.first());
	polylinePoints.append(polylinePoints.back());

	lassoShape()->setPoints(polylinePoints);
}

void SelectionPickerActor::updatePolygon()
{
	if (_mousePositions.size() == 0)
		return;

	//qDebug() << "Update lasso" << p0 << p1;

	QVector<QVector3D> positions;

	for (auto mousePosition : _mousePositions) {
		const auto pWorld = _renderer->screenToWorld(modelViewMatrix(), mousePosition);
		positions.append(QVector3D(pWorld.x(), pWorld.y(), 0.f));
	}

	QVector<PolylinePoint2D> polylinePoints;

	
	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _outlineLineWidth));
	}

	polylinePoints.insert(0, polylinePoints.first());
	polylinePoints.append(polylinePoints.back());

	polygonShape()->setPoints(polylinePoints);
}
*/