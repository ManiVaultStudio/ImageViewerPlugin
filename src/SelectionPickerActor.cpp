#include "SelectionPickerActor.h"
#include "Renderer.h"
#include "SelectionImageQuad.h"
#include "Polyline2D.h"

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

	addShape<Polyline2D>("Rectangle");
	addShape<Polyline2D>("Brush");
	addShape<Polyline2D>("Lasso");
	addShape<Polyline2D>("Polygon");
}

void SelectionPickerActor::initialize()
{
	Actor::initialize();

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
	
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
		{
			rectangleShape()->reset();
			rectangleShape()->hide();
			break;
		}

		case SelectionType::Brush:
		{
			brushShape()->reset();
			brushShape()->hide();
			break;
		}

		case SelectionType::Lasso:
		{
			lassoShape()->reset();
			lassoShape()->hide();
			break;
		}

		case SelectionType::Polygon:
		{
			polygonShape()->reset();
			polygonShape()->hide();
			break;
		}

		default:
			break;
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
				rectangleShape()->reset();
			}
			break;
		}

		case SelectionType::Brush:
		{
			//brushShape()->texture("Polyline")
			break;
		}

		case SelectionType::Lasso:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				endSelection();
				lassoShape()->reset();
			}
			break;
		}

		case SelectionType::Polygon:
		{
			if (mouseEvent->button() == Qt::RightButton) {
				endSelection();
				polygonShape()->reset();
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

				if (_mousePositions.size() >= 2)
					updateRectangle();
			}
			break;
		}

		case SelectionType::Brush:
		{
			addMousePosition(mouseEvent->pos());
			updateBrush();
			break;
		}

		case SelectionType::Lasso:
		{
			if (mouseEvent->buttons() & Qt::LeftButton)
			{
				addMousePosition(mouseEvent->pos());

				if (_mousePositions.size() >= 2)
					updateLasso();
			}
			break;
		}

		case SelectionType::Polygon:
		{
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

QMenu* SelectionPickerActor::contextMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction		= new QAction("Rectangle");
	auto* brushSelectionAction			= new QAction("Brush");
	auto* lassoSelectionAction			= new QAction("Lasso", this);
	auto* polygonSelectionAction		= new QAction("Polygon", this);
	auto* selectNoneAction				= new QAction("Select none");
	auto* selectAllAction				= new QAction("Select all");
	auto* invertSelectionAction			= new QAction("Invert");

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

Polyline2D* SelectionPickerActor::rectangleShape()
{
	return shape<Polyline2D>("Rectangle");
}

Polyline2D* SelectionPickerActor::brushShape()
{
	return shape<Polyline2D>("Brush");
}

Polyline2D* SelectionPickerActor::lassoShape()
{
	return shape<Polyline2D>("Lasso");
}

Polyline2D* SelectionPickerActor::polygonShape()
{
	return shape<Polyline2D>("Polygon");
}

void SelectionPickerActor::startSelection()
{
	switch (_selectionType)
	{
		case SelectionType::None:
			break;

		case SelectionType::Rectangle:
			rectangleShape()->show();
			break;

		case SelectionType::Brush:
			brushShape()->show();
			break;

		case SelectionType::Lasso:
			lassoShape()->show();
			break;

		case SelectionType::Polygon:
			polygonShape()->show();
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
}

void SelectionPickerActor::addMousePosition(const QPoint& point)
{
	_mousePositions.append(point);
	_positions.append(_renderer->screenToWorld(modelViewMatrix(), point));
}

void SelectionPickerActor::updateRectangle()
{
	if (_mousePositions.size() < 2)
		return;

	//qDebug() << "Update rectangle" << p0 << p1;

	QVector<QVector3D> positions;

	const auto start	= _renderer->screenToWorld(modelViewMatrix(), _mousePositions.first());
	const auto end		= _renderer->screenToWorld(modelViewMatrix(), _mousePositions.back());

	positions.append(QVector3D(start.x(), start.y(), 0.f));
	positions.append(QVector3D(start.x(), start.y(), 0.f));
	positions.append(QVector3D(end.x(), start.y(), 0.f));
	positions.append(QVector3D(end.x(), end.y(), 0.f));
	positions.append(QVector3D(start.x(), end.y(), 0.f));
	positions.append(QVector3D(start.x(), start.y(), 0.f));

	QVector<PolylinePoint2D> polylinePoints;

	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _outlineLineWidth));
	}

	polylinePoints.insert(0, polylinePoints.first());
	polylinePoints.append(polylinePoints.back());

	rectangleShape()->setPoints(polylinePoints);
}

void SelectionPickerActor::updateBrush()
{
	if (_mousePositions.size() == 0)
		return;

	//qDebug() << "Update brush";

	QVector<QVector3D> positions;

	const auto brushCenter	= _positions.back();
	const auto noSegments	= 128u;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	const auto brushRadius = _brushRadius * renderer()->zoom();

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta = 2.0f * M_PI * float(s) / float(noSegments);
		const auto x = brushRadius * cosf(theta);
		const auto y = brushRadius * sinf(theta);

		positions.append(QVector3D(brushCenter.x() + x, brushCenter.y() + y, 0.f));
	}

	QVector<PolylinePoint2D> polylinePoints;

	for (auto position : positions) {
		polylinePoints.push_back(PolylinePoint2D(position, QVector2D(0.f, 0.f), _outlineLineWidth));
	}

	polylinePoints.insert(0, polylinePoints.first());
	polylinePoints.append(polylinePoints.back());

	brushShape()->setPoints(polylinePoints);
}

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