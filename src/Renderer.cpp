#include "Renderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"
#include "ImageQuad.h"
#include "SelectionBounds.h"
#include "SelectionQuad.h"
#include "SelectionBufferQuad.h"
#include "SelectionOutline.h"

template SelectionBounds* Renderer::shape<SelectionBounds>(const QString& name);
template ImageQuad* Renderer::shape<ImageQuad>(const QString& name);
template SelectionQuad* Renderer::shape<SelectionQuad>(const QString& name);
template SelectionBufferQuad* Renderer::shape<SelectionBufferQuad>(const QString& name);
template SelectionOutline* Renderer::shape<SelectionOutline>(const QString& name);

Renderer::Renderer(const float& depth, ImageViewerWidget* imageViewerWidget) :
	StackedRenderer(depth),
	_imageViewerWidget(imageViewerWidget),
	_shapes(),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_brushRadius(10.0f),
	_brushRadiusDelta(1.f)
{
	createShapes();
}

void Renderer::render()
{
	if (!isInitialized())
		return;

	renderShapes();
}

void Renderer::resize(QSize renderSize)
{
	qDebug() << "Selection renderer resize";
}

void Renderer::init()
{
	StackedRenderer::init();

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

		const auto pWorld0 = _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
		const auto pWorld1 = _imageViewerWidget->screenToWorld(QPointF(1.f, 0.0f));

		shape<SelectionBounds>("SelectionBounds")->setLineWidth(1);
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

template<typename T>
T* Renderer::shape(const QString& name)
{
	return dynamic_cast<T*>(_shapes[name].get());
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

void Renderer::createShapes()
{
	//qDebug() << "Creating shapes";
	
	_shapes.insert("ImageQuad", QSharedPointer<ImageQuad>::create("ImageQuad", 3.f));
	_shapes.insert("SelectionBufferQuad", QSharedPointer<SelectionBufferQuad>::create("SelectionBufferQuad", 2.f));
	_shapes.insert("SelectionQuad", QSharedPointer<SelectionQuad>::create("SelectionQuad", 1.f));
	_shapes.insert("SelectionBounds", QSharedPointer<SelectionBounds>::create("SelectionBounds", 0.f));
	_shapes.insert("SelectionOutline", QSharedPointer<SelectionOutline>::create("SelectionOutline", 0.f));

	//_shapes["ImageQuad"]->setEnabled(false);
	//_shapes["SelectionQuad"]->setEnabled(false);
	//_shapes["SelectionBufferQuad"]->setEnabled(false);
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
		_shapes[key]->setModelViewProjection(_modelViewProjection);
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