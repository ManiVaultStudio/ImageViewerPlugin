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

template SelectionBounds* Renderer::shape<SelectionBounds>(const QString& name);
template ImageQuad* Renderer::shape<ImageQuad>(const QString& name);
template SelectionQuad* Renderer::shape<SelectionQuad>(const QString& name);
template SelectionBufferQuad* Renderer::shape<SelectionBufferQuad>(const QString& name);

Renderer::Renderer(const float& depth, ImageViewerWidget* imageViewerWidget) :
	StackedRenderer(depth),
	_imageViewerWidget(imageViewerWidget)
{
	createShapes();
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

void Renderer::render()
{
	if (!isInitialized())
		return;

	renderShapes();
}

void Renderer::resize(QSize renderSize)
{
	qDebug() << "Selection renderer resize";

	const auto pWorld0 = _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
	const auto pWorld1 = _imageViewerWidget->screenToWorld(QPointF(4.f, 0.0f));

	shape<SelectionBounds>("SelectionBounds")->setLineWidth((pWorld1 - pWorld0).length());
}

void Renderer::setColorImage(std::shared_ptr<QImage> colorImage)
{
	auto* imageQuadShape = shape<ImageQuad>("ImageQuad");

	const auto previousImageSize = imageQuadShape->size();

	imageQuadShape->setImage(colorImage);

	if (previousImageSize != colorImage->size()) {
		shape<SelectionBufferQuad>("SelectionBufferQuad")->setSize(colorImage->size());

		const auto brushRadius = 0.05f * static_cast<float>(std::min(colorImage->width(), colorImage->height()));

		selectionBufferQuad()->setBrushRadius(brushRadius);
		selectionBufferQuad()->setBrushRadiusDelta(0.2f * brushRadius);
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

template<typename T>
T* Renderer::shape(const QString& name)
{
	return dynamic_cast<T*>(_shapes[name].get());
}

bool Renderer::isInitialized() const
{
	return _shapes["ImageQuad"]->isInitialized();
}

void Renderer::createShapes()
{
	//qDebug() << "Creating shapes";
	
	_shapes.insert("ImageQuad", QSharedPointer<ImageQuad>::create("ImageQuad", 3.f));
	_shapes.insert("SelectionBufferQuad", QSharedPointer<SelectionBufferQuad>::create("SelectionBufferQuad", 2.f));
	_shapes.insert("SelectionQuad", QSharedPointer<SelectionQuad>::create("SelectionQuad", 1.f));
	_shapes.insert("SelectionBounds", QSharedPointer<SelectionBounds>::create("SelectionBounds", 0.f));

	//_shapes["ImageQuad"]->setEnabled(false);
	//_shapes["SelectionQuad"]->setEnabled(false);
	//_shapes["SelectionBufferQuad"]->setEnabled(false);
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

/*
void Renderer::renderOutline()
{
	 TODO
	if (_imageViewerWidget->interactionMode() != InteractionMode::Selection)
		return;

	auto outlineProgram = shaderProgram("Outline");

	if (outlineProgram->bind()) {
		const auto mousePositions = _imageViewerWidget->mousePositions();

		QMatrix4x4 transform;

		transform.ortho(_imageViewerWidget->rect());

		outlineProgram->setUniformValue("stippleTexture", 0);
		outlineProgram->setUniformValue("transform", _modelViewProjection);
		outlineProgram->setUniformValue("color", _outlineColor);

		auto* outlineVBO	= vbo("Outline").get();
		auto* outlineVAO	= vao("Outline").get();

		auto outlineStippleTexture = texture("OutlineStipple");

		outlineStippleTexture->bind();
		{
			auto screenToWorld = [&](const QVector<QVector2D>& screenPoints) -> QVector<QVector2D> {
				QVector<QVector2D> worldPoints; 

				for (const auto& screenPoint : screenPoints)
				{
					const auto worldPoint = _imageViewerWidget->screenToWorld(QPointF(screenPoint.x(), screenPoint.y()));

					worldPoints.append(QVector2D(worldPoint.x(), worldPoint.y()));
				}

				return worldPoints;
			};

			QVector<QVector2D> points;

			switch (_imageViewerWidget->selectionType())
			{
				case SelectionType::Rectangle:
				{
					if (_imageViewerWidget->selecting() && mousePositions.size() >= 2) {
						const auto start	= mousePositions.front();
						const auto end		= mousePositions.back();

						points.append(QVector2D(start.x(), start.y()));
						points.append(QVector2D(end.x(), start.y()));
						points.append(QVector2D(end.x(), end.y()));
						points.append(QVector2D(start.x(), end.y()));

						drawPolyline(screenToWorld(points), outlineVBO, outlineVAO, true, _outlineLineWidth);
					}

					break;
				}

				case SelectionType::Brush:
				{
					const auto brushCenter	= _imageViewerWidget->mousePosition();
					const auto noSegments	= 128u;

					std::vector<GLfloat> vertexCoordinates;

					vertexCoordinates.resize(noSegments * 3);

					const auto brushRadius = _brushRadius * _imageViewerWidget->zoom();

					for (std::uint32_t s = 0; s < noSegments; s++) {
						const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
						const auto x		= brushRadius * cosf(theta);
						const auto y		= brushRadius * sinf(theta);

						points.append(QVector2D(brushCenter.x() + x, brushCenter.y() + y));
					}

					drawPolyline(screenToWorld(points), outlineVBO, outlineVAO, true, _outlineLineWidth);

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					if (mousePositions.size() >= 2) {
						for (const auto& mousePosition : mousePositions) {
							points.append(QVector2D(mousePosition.x(), mousePosition.y()));
						}

						drawPolyline(screenToWorld(points), outlineVBO, outlineVAO, true, _outlineLineWidth);
					}

					break;
				}

				default:
					break;
			}
		}
		outlineStippleTexture->release();

		outlineProgram->release();
	}
	
}
*/