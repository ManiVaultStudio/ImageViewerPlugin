#include "Renderer.h"
#include "ImageViewerWidget.h"

#include <QtMath>
#include <QDebug>

#include "Shaders.h"
#include "ImageQuad.h"
#include "SelectionBounds.h"
#include "SelectionQuad.h"
#include "SelectionBufferQuad.h"
#include "SelectionOutline.h"

Renderer::Renderer(QWidget* parentWidget) :
	hdps::Renderer(),
	_parentWidget(parentWidget),
	_shapes(),
	_interactionMode(InteractionMode::Selection),
	_mouseEvents(),
	_pan(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(10.0f),
	_brushRadiusDelta(1.f)
{
	createShapes();

	// Reset the view when the image size changes
	connect(shape<ImageQuad>("ImageQuad"), &ImageQuad::sizeChanged, this, [&]() { zoomExtents(); });
}

void Renderer::render()
{
	if (!isInitialized())
		return;

	renderShapes();
}

void Renderer::resize(QSize renderSize)
{
	qDebug() << "Renderer resize";

	zoomExtents();
}

void Renderer::init()
{
	//hdps::Renderer::init();

	initializeShapes();
}

void Renderer::destroy()
{
	destroyShapes();
}

bool Renderer::isInitialized() const
{
	return _shapes["ImageQuad"]->isInitialized();
}

QVector<QSharedPointer<QMouseEvent>> Renderer::mouseEvents() const
{
	return _mouseEvents;
}

void Renderer::mousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event";

	_mouseEvents.clear();
	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMousePressEvents())
			shape->onMousePressEvent(mouseEvent);
	}
}

void Renderer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event";

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMouseReleaseEvents())
			shape->onMouseReleaseEvent(mouseEvent);
	}

	_mouseEvents.clear();
}

void Renderer::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event";

	switch (mouseEvent->buttons())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					const auto noMouseEvents = _mouseEvents.size();

					if (noMouseEvents >= 2) {
						const auto previous	= _mouseEvents[noMouseEvents - 2];
						const auto current	= _mouseEvents[noMouseEvents - 1];
						const auto delta	= (current->pos() - previous->pos()) / _zoom;

						pan(QPointF(delta.x(), delta.y()));

						emit dirty();
					}
					
					break;
				}
			}
		}
	}

	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMouseMoveEvents())
			shape->onMouseMoveEvent(mouseEvent);
	}
}

void Renderer::mouseWheelEvent(QWheelEvent* wheelEvent)
{
	//qDebug() << "Mouse wheel event";

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMouseWheelEvents())
			shape->onMouseWheelEvent(wheelEvent);
	}

//	qDebug() << "Mouse wheel event" << interactionModeTypeName(_interactionMode);

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			auto zoomCenter = wheelEvent->posF();

			//zoomCenter.setY(_parentWidget->height() - wheelEvent->posF().y());

			if (wheelEvent->delta() < 0) {
				zoomAround(zoomCenter, 1.f - _zoomSensitivity);
			}
			else {
				zoomAround(zoomCenter, 1.f + _zoomSensitivity);
			}

			emit dirty();
			break;
		}

		case InteractionMode::Selection:
		{
			if (selectionType() == SelectionType::Brush) {
				if (wheelEvent->delta() > 0) {
					brushSizeIncrease();
				}
				else {
					brushSizeDecrease();
				}
			}

			break;
		}

		default:
			break;
	}
}

QVector3D Renderer::screenToWorld(const QMatrix4x4& modelViewMatrix, const QPointF& screenPoint) const
{
	return QVector3D(screenPoint.x(), _parentWidget->height()- screenPoint.y(), 0).unproject(modelViewMatrix, projectionMatrix(), QRect(0, 0, _parentWidget->width(), _parentWidget->height()));
}

QMatrix4x4 Renderer::viewMatrix() const
{
	QMatrix4x4 lookAt, scale;

	lookAt.lookAt(QVector3D(_pan.x(), _pan.y(), -1), QVector3D(_pan.x(), _pan.y(), 0), QVector3D(0, 1, 0));
	scale.scale(_zoom);

	return scale * lookAt;
}

QMatrix4x4 Renderer::projectionMatrix() const
{
	const auto halfSize = _parentWidget->size() / 2;

	QMatrix4x4 matrix;

	matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

	return matrix;
}

void Renderer::pan(const QPointF& delta)
{
	qDebug() << "Pan by" << delta;

	_pan.setX(_pan.x() + delta.x());
	_pan.setY(_pan.y() + delta.y());
}

float Renderer::zoom() const
{
	return _zoom;
}

void Renderer::zoomBy(const float& factor)
{
	if (factor == 0.f)
		return;

	qDebug() << "Zoom by" << factor << "to" << _zoom;

	_zoom *= factor;

	

	//_pan.setX(_pan.x() * factor);
	//_pan.setY(_pan.y() * factor);
}

void Renderer::zoomAround(const QPointF& screenPoint, const float& factor)
{
	zoomBy(factor);

	//	qDebug() << "Zoom at" << screenPoint << "by" << factor;

	const auto pWorld			= screenToWorld(viewMatrix(), screenPoint);
	const auto pAnchor			= pWorld;
	const auto pPanOld			= QVector3D(_pan.x(), _pan.y(), 0.f);
	const auto vPanOld			= pPanOld - pAnchor;
	const auto vPanNew			= factor * vPanOld;
	const auto vPanDelta		= vPanNew - vPanOld;

	pan(-QPointF(vPanDelta.x(), vPanDelta.y()));
}

void Renderer::zoomExtents()
{
	qDebug() << "Zoom extents";

	const auto imageQuadSize = shape<ImageQuad>("ImageQuad")->size();

	zoomToRectangle(QRectF(QPointF(), imageQuadSize));
}

void Renderer::zoomToRectangle(const QRectF& rectangle)
{
	if (!rectangle.isValid())
		return;

	qDebug() << "Zoom to rectangle" << rectangle;

	resetView();

	const auto center	= rectangle.center();
	const auto factorX	= (_parentWidget->width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (_parentWidget->height() - 2 * _margin) / static_cast<float>(rectangle.height());
	
	zoomBy(factorX < factorY ? factorX : factorY);
	auto* imageQuad = shape<ImageQuad>("ImageQuad");

	//pan(_zoom * -QPointF(center.x(), imageQuad->size().height() - center.y()));
	
	emit dirty();
}

void Renderer::zoomToSelection()
{
	/*
	auto* currentImageDataSet = _imageViewerPlugin->currentImages();

	if (currentImageDataSet == nullptr)
		return;

	qDebug() << "Zoom to selection";

	zoomToRectangle(QRectF(currentImageDataSet->selectionBounds(true)));
	*/
}

void Renderer::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);

	_zoom = 1.f;
}

void Renderer::setColorImage(std::shared_ptr<QImage> colorImage)
{
	auto* imageQuadShape = shape<ImageQuad>("ImageQuad");

	const auto previousImageSize = imageQuadShape->size();

	imageQuadShape->setImage(colorImage);

	if (previousImageSize != colorImage->size()) {
		shape<SelectionBufferQuad>("SelectionBufferQuad")->setSize(colorImage->size());

		const auto brushRadius = 0.05f * static_cast<float>(std::min(colorImage->width(), colorImage->height()));

		setBrushRadius(brushRadius);
		setBrushRadiusDelta(0.2f * brushRadius);

		/*
		const auto pWorld0 = _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
		const auto pWorld1 = _imageViewerWidget->screenToWorld(QPointF(1.f, 0.0f));

		shape<SelectionBounds>("SelectionBounds")->setLineWidth(1);
		*/
	}
}

void Renderer::setSelectionImage(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds)
{
	auto worldSelectionBounds = QRect(selectionBounds.left(), selectionImage->height() - selectionBounds.bottom() - 1, selectionBounds.width() + 1, selectionBounds.height() + 1);

	worldSelectionBounds.translate(QPoint(-0.5f * static_cast<float>(selectionImage->width()), -0.5f * static_cast<float>(selectionImage->height())));

	shape<SelectionQuad>("SelectionQuad")->setImage(selectionImage);
	shape<SelectionBounds>("SelectionBounds")->setBounds(worldSelectionBounds);
}

float Renderer::selectionOpacity()
{
	return shape<SelectionQuad>("SelectionQuad")->opacity();
}

void Renderer::setSelectionOpacity(const float& selectionOpacity)
{
	shape<SelectionQuad>("SelectionQuad")->setOpacity(selectionOpacity);
}

ImageQuad* Renderer::imageQuad()
{
	return shape<ImageQuad>("ImageQuad");
}

SelectionBufferQuad* Renderer::selectionBufferQuad()
{
	return shape<SelectionBufferQuad>("SelectionBufferQuad");
}

SelectionOutline* Renderer::selectionOutline()
{
	return shape<SelectionOutline>("SelectionOutline");
}

InteractionMode Renderer::interactionMode() const
{
	return _interactionMode;
}

void Renderer::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(interactionMode);

	switch (interactionMode)
	{
		case InteractionMode::Navigation:
			_parentWidget->setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
		case InteractionMode::None:
			_parentWidget->setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}

	_interactionMode = interactionMode;
}

SelectionType Renderer::selectionType() const
{
	return _selectionType;
}

void Renderer::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	_selectionType = selectionType;

	qDebug() << "Set selection type to" << selectionTypeName(_selectionType);

	emit selectionTypeChanged(_selectionType);
}

SelectionModifier Renderer::selectionModifier() const
{
	return _selectionModifier;
}

void Renderer::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	_selectionModifier = selectionModifier;

	qDebug() << "Set selection modifier to" << selectionModifierName(selectionModifier);

	emit selectionModifierChanged(_selectionModifier);
}

float Renderer::brushRadius() const
{
	return _brushRadius;
}

void Renderer::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 100000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius to" << QString::number(_brushRadius, 'f', 1);

	emit brushRadiusChanged(_brushRadius);
}

float Renderer::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void Renderer::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.1f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = boundBrushRadiusDelta;

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;

	emit brushRadiusDeltaChanged(_brushRadiusDelta);
}

void Renderer::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void Renderer::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

void Renderer::onShapeChanged(Shape* shape)
{
	//qDebug() << "Shape" << shape->name() << "changed";

	emit dirty();
}

void Renderer::addShape(const QString& name, QSharedPointer<Shape> shape)
{
	_shapes.insert(name, shape);

	connect(shape.get(), &Shape::changed, this, &Renderer::onShapeChanged);
}

void Renderer::createShapes()
{
	//qDebug() << "Creating shapes";
	
	addShape("ImageQuad", QSharedPointer<ImageQuad>::create(this, "ImageQuad", 3.f));
	addShape("SelectionBufferQuad", QSharedPointer<SelectionBufferQuad>::create(this, "SelectionBufferQuad", 2.f));
	addShape("SelectionQuad", QSharedPointer<SelectionQuad>::create(this, "SelectionQuad", 1.f));
	addShape("SelectionBounds", QSharedPointer<SelectionBounds>::create(this, "SelectionBounds", 0.f));
	addShape("SelectionOutline", QSharedPointer<SelectionOutline>::create(this, "SelectionOutline", 0.f));

	//_shapes["ImageQuad"]->setEnabled(false);
	_shapes["SelectionBufferQuad"]->setEnabled(false);
	_shapes["SelectionQuad"]->setEnabled(false);
	_shapes["SelectionBounds"]->setEnabled(false);
	//_shapes["SelectionOutline"]->setEnabled(false);
}

void Renderer::initializeShapes()
{
	//qDebug() << "Initializing" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->initialize();
	}
}

void Renderer::renderShapes()
{
	//qDebug() << "Render" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->render();
	}
}

void Renderer::destroyShapes()
{
	//qDebug() << "Destroying" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->destroy();
	}
}