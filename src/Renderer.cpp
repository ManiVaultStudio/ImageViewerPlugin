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

Renderer::Renderer(QWidget* parent) :
	hdps::Renderer(),
	_parent(parent),
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

void Renderer::mousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event";

	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMousePressEvents())
			shape->onMousePressEvent(mouseEvent);
	}
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
					const auto previous	= _mouseEvents[_mouseEvents.size() - 2];
					const auto current	= _mouseEvents[_mouseEvents.size() - 1];
					const auto delta	= current->pos() - previous->pos();
					
					pan(QPointF(delta.x(), delta.y()));

					emit dirty();
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

void Renderer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event";

	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _shapes.keys()) {
		auto shape = _shapes[key];

		if (shape->isActive() && shape->handlesMouseReleaseEvents())
			shape->onMouseReleaseEvent(mouseEvent);
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
			const auto world_x = (wheelEvent->posF().x() - _pan.x()) / _zoom;
			const auto world_y = (wheelEvent->posF().y() - _pan.y()) / _zoom;

			auto zoomCenter = wheelEvent->posF();

			//zoomCenter.setY(height() - wheelEvent->posF().y());

			if (wheelEvent->delta() > 0) {
				zoomAt(zoomCenter, 1.f - _zoomSensitivity);
			}
			else {
				zoomAt(zoomCenter, 1.f + _zoomSensitivity);
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

QVector3D Renderer::screenToWorld(const QMatrix4x4& modelViewMatrix, const QMatrix4x4& projectionMatrix, const QPointF& screenPoint) const
{
	return QVector3D(screenPoint.x(), _parent->height()- screenPoint.y(), 0).unproject(modelViewMatrix, projectionMatrix, QRect(0, 0, _parent->width(), _parent->height()));
}

QMatrix4x4 Renderer::modelView() const
{
	QMatrix4x4 model, view;

	model.scale(_zoom, _zoom, 1.0f);
	model.translate(_pan.x(), _pan.y());
	view.lookAt(QVector3D(0, 0, -1), QVector3D(0, 0, 0), QVector3D(0, -1, 0));

	return view * model;
}

QMatrix4x4 Renderer::projection() const
{
	const auto halfSize = _parent->size() / 2;

	QMatrix4x4 projection;

	projection.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -100.0f, +100.0f);

	return projection;
}

void Renderer::pan(const QPointF& delta)
{
	qDebug() << "Pan" << delta;

	_pan.setX(_pan.x() + (delta.x() / _zoom));
	_pan.setY(_pan.y() + (delta.y() / _zoom));
}

float Renderer::zoom() const
{
	return _zoom;
}

void Renderer::zoomBy(const float& factor)
{
	_zoom *= factor;

	qDebug() << "Zoom by" << _zoom;

	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);
}

void Renderer::zoomAt(const QPointF& screenPosition, const float& factor) {

	qDebug() << "Zoom at" << screenPosition << factor;

	//pan(QPointF(-screenPosition.x(), -screenPosition.y()));
	zoomBy(factor);
	//pan(QPointF(screenPosition.x(), screenPosition.y()));
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
	const auto factorX	= (_parent->width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (_parent->height() - 2 * _margin) / static_cast<float>(rectangle.height());
	
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
	const auto worldSelectionBounds = QRect(selectionBounds.left(), selectionImage->height() - selectionBounds.bottom() - 1, selectionBounds.width() + 1, selectionBounds.height() + 1);

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
			_parent->setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
		case InteractionMode::None:
			_parent->setCursor(Qt::ArrowCursor);
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
	//_shapes["SelectionBufferQuad"]->setEnabled(false);
	//_shapes["SelectionQuad"]->setEnabled(false);
	//_shapes["SelectionBounds"]->setEnabled(false);
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

	//qDebug() << modelView();
	//qDebug() << projection();

	for (auto key : _shapes.keys()) {
		_shapes[key]->setModelView(modelView());
		_shapes[key]->setProjectionMatrix(projection());
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