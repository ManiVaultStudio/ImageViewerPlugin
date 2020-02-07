#include "SelectionPickerActor.h"
#include "PolylineProp.h"
#include "InterimSelectionProp.h"
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
	_brushRadius(40.0f),
	_brushRadiusDelta(5.f),
	_selecting(false)
{
	_registeredEvents |= static_cast<int>(ActorEvent::MousePress);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseRelease);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseMove);
	_registeredEvents |= static_cast<int>(ActorEvent::MouseWheel);
	_registeredEvents |= static_cast<int>(ActorEvent::KeyPress);
	_registeredEvents |= static_cast<int>(ActorEvent::KeyRelease);

	addProp<PolylineProp>("RectangleProp");
	addProp<PolylineProp>("BrushProp");
	addProp<PolylineProp>("LassoProp");
	addProp<PolylineProp>("PolygonSegmentsProp");
	addProp<PolylineProp>("PolygonClosingSegmentProp");
	addProp<InterimSelectionProp>("InterimSelectionProp");
}

void SelectionPickerActor::initialize()
{
	Actor::initialize();

	rectangleProp()->setLineWidth(2.5f);
	rectangleProp()->setLineColor(renderer()->colorByName("SelectionOutline", 200));

	brushProp()->setLineWidth(2.5f);

	polygonSegmentsProp()->setClosed(false);
	polygonSegmentsProp()->setLineWidth(3.5f);
	polygonSegmentsProp()->setLineColor(renderer()->colorByName("SelectionOutline", 200));
	
	polygonClosingSegmentProp()->setClosed(false);
	polygonClosingSegmentProp()->setLineWidth(2.0f);
	polygonClosingSegmentProp()->setLineColor(renderer()->colorByName("SelectionOutline", 100));
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

void SelectionPickerActor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			if (mouseEvent->button() == Qt::LeftButton) {
				startSelection();
				addMouseEvent(mouseEvent);
				break;
			}

		case SelectionType::Brush:
			if (mouseEvent->button() == Qt::LeftButton) {
				startSelection();
				addMouseEvent(mouseEvent);
				break;
			}

		case SelectionType::Lasso:
			break;

		case SelectionType::Polygon:
			if (mouseEvent->button() == Qt::LeftButton) {
				addMouseEvent(mouseEvent);
				break;
			}

		default:
			break;
	}

	update();
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
			}
			break;
		}

		default:
			break;
	}
	
	update();
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
			}
			break;
		}

		case SelectionType::Brush:
		{
			addMouseEvent(mouseEvent);
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
			}

			if (_mouseEvents.size() >= 2)
			{
				const auto pScreen = QVector2D(mouseEvent->pos());
				_mouseEvents.last() = MouseEvent(pScreen, renderer()->screenPointToWorldPosition(modelViewMatrix(), pScreen));
			}
			break;
		}

		default:
			break;
	}

	update();
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

	update();
}

void SelectionPickerActor::startSelection()
{
	qDebug() << "Start selection" << name();

	_mouseEvents.clear();

	_selecting = true;

	update();
}

void SelectionPickerActor::endSelection()
{
	qDebug() << "End selection" << name();

	_mouseEvents.clear();
	
	_selecting = false;
	
	update();
}

void SelectionPickerActor::show()
{
	Actor::show();
	/*
	_mouseEvents.clear();

	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}
	*/
}

void SelectionPickerActor::hide()
{
	Actor::hide();

	_mouseEvents.clear();
}

void SelectionPickerActor::update()
{
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
			updateSelectionLasso();
			break;

		case SelectionType::Polygon:
			updateSelectionPolygon();
			break;

		default:
			break;
	}

	interimSelectionProp()->update();

	emit changed(this);
}

void SelectionPickerActor::updateSelectionRectangle()
{
	QVector<QVector3D> points;

	// No need to draw a rectangle if less than two mouse events were recorded
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
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), end.y(), 0.f));

	rectangleProp()->setPoints(points);
}

void SelectionPickerActor::updateSelectionBrush()
{
	QVector<QVector3D> points;

	// Determine the center of the brush in screen coordinates
	const auto mousePosition	= renderer()->parentWidget()->mapFromGlobal(QCursor::pos());
	const auto pCenter			= QVector2D(mousePosition.x(), renderer()->viewSize().height() - mousePosition.y());
	const auto noSegments		= 128u;

	std::vector<GLfloat> vertexCoordinates;

	// Allocate vertices buffer
	vertexCoordinates.resize(noSegments * 3);

	// Generate polyline points in screen coordinates
	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta	= 2.0f * M_PI * float(s) / static_cast<float>(noSegments);
		const auto pBrush	= QVector2D(_brushRadius * cosf(theta), _brushRadius * sinf(theta));

		points.append(pCenter + pBrush);
	}
	
	brushProp()->setPoints(points);

	const auto leftButtonDown = QGuiApplication::mouseButtons() & Qt::LeftButton;
	
	// Change the line color when the left mouse button is down
	brushProp()->setLineColor(leftButtonDown ? renderer()->colorByName("SelectionOutline", 200) : renderer()->colorByName("SelectionOutline", 80));
}

void SelectionPickerActor::updateSelectionLasso()
{
}

void SelectionPickerActor::updateSelectionPolygon()
{
	const auto noMouseEvents = _mouseEvents.size();

	// A polyline for n segments and one for the segment that closes the polygon
	QVector<QVector3D> segmentsPoints, closingSegmentPoints;

	// Handle edge case of only two mouse points
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

	interimSelectionProp()->setImageSize(_imageSize);

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

	_selectionType = selectionType;

	qDebug() << "Set selection type to" << selectionTypeName(_selectionType);

	_selecting = false;

	rectangleProp()->setVisible(_selectionType == SelectionType::Rectangle);
	brushProp()->setVisible(_selectionType == SelectionType::Brush);
	lassoProp()->setVisible(_selectionType == SelectionType::Lasso);
	polygonSegmentsProp()->setVisible(_selectionType == SelectionType::Polygon);
	polygonClosingSegmentProp()->setVisible(_selectionType == SelectionType::Polygon);

	update();

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

PolylineProp* SelectionPickerActor::rectangleProp()
{
	return propByName<PolylineProp>("RectangleProp");
}

PolylineProp* SelectionPickerActor::brushProp()
{
	return propByName<PolylineProp>("BrushProp");
}

PolylineProp* SelectionPickerActor::lassoProp()
{
	return propByName<PolylineProp>("LassoProp");
}

PolylineProp* SelectionPickerActor::polygonSegmentsProp()
{
	return propByName<PolylineProp>("PolygonSegmentsProp");
}

PolylineProp* SelectionPickerActor::polygonClosingSegmentProp()
{
	return propByName<PolylineProp>("PolygonClosingSegmentProp");
}

InterimSelectionProp* SelectionPickerActor::interimSelectionProp()
{
	return propByName<InterimSelectionProp>("InterimSelectionProp");
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
	connect(invertSelectionAction, &QAction::triggered, this, &SelectionPickerActor::selectInvert);

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

bool SelectionPickerActor::isSelecting() const
{
	return _selecting;
}