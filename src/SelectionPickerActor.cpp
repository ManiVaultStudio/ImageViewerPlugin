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
#include <QDebug>

#include <QtMath>

SelectionPickerActor::SelectionPickerActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageSize(),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(1.0f),
	_brushRadiusDelta(1.f),
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
	addProp<SelectionBrushProp>("SelectionBrushProp");
	addProp<SelectionLassoProp>("SelectionLassoProp");
	addProp<SelectionPolygonProp>("SelectionPolygonProp");
}

void SelectionPickerActor::initialize()
{
	Actor::initialize();

	/*
	rectangleShape()->setLineWidth(_outlineLineWidth);
	brushShape()->setLineWidth(_outlineLineWidth);
	lassoShape()->setLineWidth(_outlineLineWidth);
	polygonShape()->setLineWidth(_outlineLineWidth);

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), QColor(255, 0, 0));

	auto texture = QSharedPointer<QOpenGLTexture>::create(textureImage);

	texture->create();
	texture->setWrapMode(QOpenGLTexture::Repeat);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	rectangleShape()->setTexture("Polyline", texture);
	brushShape()->setTexture("Polyline", texture);
	lassoShape()->setTexture("Polyline", texture);
	polygonShape()->setTexture("Polyline", texture);
	*/
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
	const auto boundBrushRadius = qBound(1.0f, 100000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius to" << QString::number(_brushRadius, 'f', 1);

	emit brushRadiusChanged(_brushRadius);
	emit changed(this);
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
			/*
			if (mouseEvent->button() == Qt::RightButton) {
				endSelection();
				polygonShape()->reset();
			}
			*/
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
			selectionBrushProp()->setBrushCenter(mouseEvent->pos());
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
			/*
			if (_mousePositions.size() == 1)
			{
				addMousePosition(mouseEvent->pos());
				updatePolygon();
			}

			if (_mousePositions.size() >= 2)
			{
				_mousePositions.back() = mouseEvent->pos();
				updatePolygon();
			}
			*/
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
			selectionRectangleProp()->show();
			break;

		case SelectionType::Brush:
			selectionBrushProp()->show();
			break;

		case SelectionType::Lasso:
			//lassoShape()->show();
			break;

		case SelectionType::Polygon:
			//polygonShape()->show();
			break;

		default:
			break;
	}

	_mousePositions.clear();
	_positions.clear();
}

void SelectionPickerActor::endSelection()
{
	_mousePositions.clear();
	_positions.clear();

	for (auto propName : _props.keys())
	{
		_props.value(propName)->hide();
	}
}

void SelectionPickerActor::addMousePosition(const QPoint& point)
{
	_mousePositions.append(point);
	_positions.append(_renderer->screenToWorld(modelViewMatrix(), point));
}
PolylineProp* SelectionPickerActor::selectionRectangleProp()
{
	return propByName<PolylineProp>("SelectionRectangleProp");
}

SelectionBrushProp* SelectionPickerActor::selectionBrushProp()
{
	return propByName<SelectionBrushProp>("SelectionBrushProp");
}

void SelectionPickerActor::updateSelectionRectangle()
{
	QVector<QVector3D> points;

	if (_mousePositions.size() < 2)
	{
		selectionRectangleProp()->setPoints(points);
		return;
	}

	auto rectangle = QRect(_mousePositions.first(), _mousePositions.back()).normalized();

	const auto start	= renderer()->screenToWorld(modelViewMatrix(), rectangle.topLeft());
	const auto end		= renderer()->screenToWorld(modelViewMatrix(), rectangle.bottomRight());

	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), start.y(), 0.f));
	points.append(QVector3D(end.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), end.y(), 0.f));
	points.append(QVector3D(start.x(), start.y(), 0.f));

	selectionRectangleProp()->setPoints(points);
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