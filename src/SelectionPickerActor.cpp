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
	_mousePositions(),
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

void SelectionPickerActor::initialize()
{
	Actor::initialize();

	rectangleProp()->setLineWidth(1);
	selectionBrushProp()->setLineWidth(renderer()->lineWidthNDC(5.f));
	polygonSegmentsProp()->setLineWidth(renderer()->lineWidthNDC(5.f));
	polygonClosingSegmentProp()->setLineWidth(renderer()->lineWidthNDC(2.f));

	polygonSegmentsProp()->setLineColor(QColor(255, 165, 0, 220));
	polygonClosingSegmentProp()->setLineColor(QColor(255, 165, 0, 50));
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
	addMousePosition(mouseEvent->pos());
	startSelection();
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
				endSelection();
				updateSelectionRectangle();
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
				addMousePosition(mouseEvent->pos());
				updateSelectionRectangle();
			}
			break;
		}

		case SelectionType::Brush:
		{
			addMousePosition(mouseEvent->pos());
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
			if (_mousePositions.size() == 1)
			{
				addMousePosition(mouseEvent->pos());
				updateSelectionPolygon();
			}

			if (_mousePositions.size() >= 2)
			{
				_mousePositions.back() = mouseEvent->pos();
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
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			rectangleProp()->show();
			break;

		case SelectionType::Brush:
			selectionBrushProp()->show();
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

	//_mousePositions.clear();
	//_positions.clear();
}

void SelectionPickerActor::endSelection()
{
	_mousePositions.clear();
	_positions.clear();
	/*
	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}
	*/
}

void SelectionPickerActor::addMousePosition(const QPoint& point)
{
	_mousePositions.append(point);
	_positions.append(_renderer->screenPointToWorldPosition(modelViewMatrix(), point));
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

void SelectionPickerActor::updateSelectionRectangle()
{
	QVector<QVector3D> points;

	if (_mousePositions.size() < 2)
	{
		rectangleProp()->setPoints(points);
		emit changed(this);
		return;
	}

	// Get first and last recorded mouse position in world coordinates
	const auto pWorldA	= _positions.first();
	const auto pWorldB	= _positions.back();

	// Create a normalized rectangle
	auto rectangle = QRectF(QPointF(pWorldA.x(), pWorldA.y()), QPointF(pWorldB.x(), pWorldB.y())).normalized();

	// Compute rectangle start and end in screen coordinates
	const auto topLeft		= rectangle.topLeft();
	const auto bottomRight	= rectangle.bottomRight();
	const auto start		= renderer()->worldPositionToScreenPoint(QVector3D(topLeft.x(), topLeft.y(), 0.f));
	const auto end			= renderer()->worldPositionToScreenPoint(QVector3D(bottomRight.x(), bottomRight.y(), 0.f));

	// Create polyline points
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));

	rectangleProp()->setPoints(points);

	emit changed(this);
}

void SelectionPickerActor::updateSelectionBrush()
{
	QVector<QVector3D> points;

	const auto pCenter		= QVector2D(_mousePositions.back().x(), _mousePositions.back().y());
	const auto noSegments	= 128u;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
		const auto pBrush	= QVector2D(_brushRadius * cosf(theta), _brushRadius * sinf(theta));

		points.append(renderer()->screenPointToNormalizedScreenPoint(pCenter + pBrush));
	}

	points.insert(0, points.back());

	const auto leftButtonDown = QGuiApplication::mouseButtons() & Qt::LeftButton;

	selectionBrushProp()->setLineColor(leftButtonDown ? QColor(255, 165, 0, 150) : QColor(255, 165, 0, 50));
	//selectionBrushProp()->setLineWidth(leftButtonDown ? renderer()->lineWidthNDC(3.0f) : renderer()->lineWidthNDC(1.5f));
	selectionBrushProp()->setPoints(points);

	emit changed(this);
}

void SelectionPickerActor::updateSelectionLasso()
{
	emit changed(this);
}

void SelectionPickerActor::updateSelectionPolygon()
{
	const auto noMousePositions = _mousePositions.size();

	if (noMousePositions < 2)
	{
		polygonSegmentsProp()->setPoints(QVector<QVector3D>());
		polygonClosingSegmentProp()->setPoints(QVector<QVector3D>());
		return;
	}

	if (noMousePositions >= 2) {
		QVector<QVector3D> points;

		for (auto position : _positions) {
			points.append(renderer()->worldPositionToNormalizedScreenPoint(position));
		}

		polygonSegmentsProp()->setPoints(points);
	}
	
	if (noMousePositions >= 3) {
		QVector<QVector3D> points;

		points.append(renderer()->worldPositionToNormalizedScreenPoint(_positions[1]));
		points.append(renderer()->worldPositionToNormalizedScreenPoint(_positions[0]));
		points.append(renderer()->worldPositionToNormalizedScreenPoint(_positions[noMousePositions - 2]));
		points.append(renderer()->worldPositionToNormalizedScreenPoint(_positions[noMousePositions - 1]));

		polygonClosingSegmentProp()->setPoints(points);
	}

	emit changed(this);
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