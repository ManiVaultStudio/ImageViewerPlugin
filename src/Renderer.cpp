#include "Renderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"
#include "ImageQuad.h"
#include "SelectionBounds.h"
#include "SelectionQuad.h"

template SelectionBounds* Renderer::shape<SelectionBounds>(const QString& name);
template ImageQuad* Renderer::shape<ImageQuad>(const QString& name);
template SelectionQuad* Renderer::shape<SelectionQuad>(const QString& name);

Renderer::Renderer(const float& depth, ImageViewerWidget* imageViewerWidget) :
	StackedRenderer(depth),
	_imageViewerWidget(imageViewerWidget),
	_bufferColor(255, 153, 0, 70),
	_selectionColor(255, 0, 0, 153),
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f)
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

	/*
	const auto pWorld0 = _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
	const auto pWorld1 = _imageViewerWidget->screenToWorld(QPointF(1.f, 0.0f));

	_selectionBounds.setLineWidth((pWorld1 - pWorld0).length());
	_selectionBounds.setModelViewProjection(_modelViewProjection);
	_selectionBounds.render();

	_imageQuad.setModelViewProjection(_modelViewProjection);
	_imageQuad.render();
	*/
}

void Renderer::resize(QSize renderSize)
{
	qDebug() << "Selection renderer resize";
}

/*
	auto createFBO = false;

	if (!_fbos.contains("SelectionBuffer")) {
		createFBO = true;
	}
	else {
		if (size != fbo("SelectionBuffer")->size()) {
			createFBO = true;
		}
	}

	if (createFBO) {
		_fbos.insert("SelectionBuffer", QSharedPointer<QOpenGLFramebufferObject>::create(size.width(), size.height()));
	}

	setSize(size);
	*/

void Renderer::updateSelectionBuffer()
{
	/*
	//qDebug() << "Update selection buffer";

	auto selectionFBO = fbo("SelectionBuffer");

	if (!selectionFBO->bind())
		return;
	
	glViewport(0, 0, selectionFBO->width(), selectionFBO->height());

	QMatrix4x4 transform;

	auto width = selectionFBO->width();

	transform.ortho(0.0f, selectionFBO->width(), 0.0f, selectionFBO->height(), -1.0f, +1.0f);

	auto quadVAO = vao("Quad");

	const auto mousePositions = _imageViewerWidget->mousePositionsWorld();

	quadVAO->bind();
	{
		auto selectionBufferProgram = shaderProgram("SelectionBuffer");

		if (selectionBufferProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, selectionFBO->texture());

			selectionBufferProgram->setUniformValue("pixelSelectionTexture", 0);
			selectionBufferProgram->setUniformValue("transform", transform);
			selectionBufferProgram->setUniformValue("selectionType", static_cast<int>(_imageViewerWidget->selectionType()));
			selectionBufferProgram->setUniformValue("imageSize", static_cast<float>(selectionFBO->size().width()), static_cast<float>(selectionFBO->size().height()));

			switch (_imageViewerWidget->selectionType())
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= mousePositions.front();
					const auto rectangleBottomRight		= mousePositions.back();
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(selectionFBO->width()), rectangleTopLeft.y() / static_cast<float>(selectionFBO->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(selectionFBO->width()), rectangleBottomRight.y() / static_cast<float>(selectionFBO->height()));

					auto rectangleUV	= std::make_pair(rectangleTopLeftUV, rectangleBottomRightUV);
					auto topLeft		= QVector2D(rectangleTopLeftUV.x(), rectangleTopLeftUV.y());
					auto bottomRight	= QVector2D(rectangleBottomRightUV.x(), rectangleBottomRightUV.y());

					if (rectangleBottomRightUV.x() < rectangleTopLeftUV.x()) {
						topLeft.setX(rectangleBottomRightUV.x());
						bottomRight.setX(rectangleTopLeftUV.x());
					}

					if (rectangleBottomRightUV.y() < rectangleTopLeftUV.y()) {
						topLeft.setY(rectangleBottomRightUV.y());
						bottomRight.setY(rectangleTopLeftUV.y());
					}

					selectionBufferProgram->setUniformValue("rectangleTopLeft", topLeft);
					selectionBufferProgram->setUniformValue("rectangleBottomRight", bottomRight);

					break;
				}

				case SelectionType::Brush:
				{
					const auto brushCenter			= mousePositions[mousePositions.size() - 1];
					const auto previousBrushCenter	= mousePositions.size() > 1 ? mousePositions[mousePositions.size() - 2] : brushCenter;

					selectionBufferProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					selectionBufferProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					selectionBufferProgram->setUniformValue("brushRadius", _brushRadius);

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					QList<QVector2D> points;

					points.reserve(static_cast<std::int32_t>(mousePositions.size()));

					for (const auto& mousePosition : mousePositions) {
						points.push_back(QVector2D(mousePosition.x(), mousePosition.y()));
					}

					selectionBufferProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
					selectionBufferProgram->setUniformValue("noPoints", static_cast<int>(points.size()));

					break;
				}

				default:
					break;
			}

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			selectionBufferProgram->release();
		}
	}
	quadVAO->release();

	selectionFBO->release();
	*/
}

void Renderer::resetSelectionBuffer()
{
	/*
	qDebug() << "Reset";

	auto selectionFBO = fbo("SelectionBuffer");
	
	if (!selectionFBO->bind())
		return;

	glViewport(0, 0, selectionFBO->width(), selectionFBO->height());
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	selectionFBO->release();
	*/
}

void Renderer::setSelectionImage(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds)
{
	const auto worldSelectionBounds = QRect(selectionBounds.left(), selectionImage->height() - selectionBounds.bottom() - 1, selectionBounds.width() + 1, selectionBounds.height() + 1);

	shape<SelectionQuad>("SelectionQuad")->setImage(selectionImage);
	shape<SelectionBounds>("SelectionBounds")->setBounds(worldSelectionBounds);
}

float Renderer::selectionOpacity() const
{
	return _selectionColor.alphaF();
}

void Renderer::setSelectionOpacity(const float& selectionOpacity)
{
	if (selectionOpacity == _selectionColor.alphaF())
		return;

	_selectionColor.setAlphaF(selectionOpacity);

	qDebug() << "Set selection opacity" << selectionOpacity;

	emit selectionOpacityChanged(_selectionColor.alphaF());
}

float Renderer::brushRadius() const
{
	return _brushRadius;
}

void Renderer::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius" << brushRadius;
}

float Renderer::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void Renderer::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;
}

void Renderer::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void Renderer::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

std::shared_ptr<QImage> Renderer::selectionImage() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	return std::make_shared<QImage>(selectionFBO->toImage());
}

template<typename T>
T* Renderer::shape(const QString& name)
{
	return dynamic_cast<T*>(_shapes[name].get());
}

bool Renderer::isInitialized() const
{
	// TODO
	return true;

	auto selectionFBO = fbo("SelectionBuffer");

	if (selectionFBO.get() == nullptr)
		return false;

	return selectionFBO->isValid();
}

/*
void Renderer::createShaderPrograms()
{
	auto overlayProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOverlayVertexShaderSource.c_str());
	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOverlayFragmentShaderSource.c_str());
	overlayProgram->link();

	_shaderPrograms.insert("Overlay", overlayProgram);

	auto outlineProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	outlineProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOutlineVertexShaderSource.c_str());
	outlineProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOutlineFragmentShaderSource.c_str());
	outlineProgram->link();

	_shaderPrograms.insert("Outline", outlineProgram);

	auto selectionBufferProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBufferVertexShaderSource.c_str());
	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBufferFragmentShaderSource.c_str());
	selectionBufferProgram->link();

	_shaderPrograms.insert("SelectionBuffer", selectionBufferProgram);

	auto selectionProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	selectionProgram->link();

	_shaderPrograms.insert("Selection", selectionProgram);
	

void Renderer::createVBOs()
{
	auto quadVBO		= QSharedPointer<QOpenGLBuffer>::create();

	quadVBO->create();
	
	_vbos.insert("Quad", quadVBO);
}

void Renderer::createVAOs()
{
	auto quadVAO	= QSharedPointer<QOpenGLVertexArrayObject>::create();

	quadVAO->create();

	_vaos.insert("Quad", quadVAO);
}
*/
void Renderer::createShapes()
{
	qDebug() << "Creating shapes";
	
	_shapes.insert("ImageQuad", QSharedPointer<ImageQuad>::create("ImageQuad", 2.f));
	_shapes.insert("SelectionQuad", QSharedPointer<SelectionQuad>::create("SelectionQuad", 1.f));
	_shapes.insert("SelectionBounds", QSharedPointer<SelectionBounds>::create("SelectionBounds", 0.f));

	//_shapes["ImageQuad"]->setEnabled(false);
	//_shapes["SelectionQuad"]->setEnabled(false);
}

void Renderer::initializeShapes()
{
	qDebug() << "Initializing" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->initialize();
	}
}

void Renderer::renderShapes()
{
	qDebug() << "Render" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->setModelViewProjection(_modelViewProjection);
		_shapes[key]->render();
	}
}

void Renderer::destroyShapes()
{
	qDebug() << "Destroying" << _shapes.size() << "shapes";

	for (auto key : _shapes.keys()) {
		_shapes[key]->destroy();
	}
}

void Renderer::renderOverlay()
{
	/* TODO
	auto selectionFBO = fbo("SelectionBuffer");

	auto overlayProgram = shaderProgram("Overlay");

	if (overlayProgram->bind()) {
		overlayProgram->setUniformValue("overlayTexture", 0);
		overlayProgram->setUniformValue("transform", _modelViewProjection);
		overlayProgram->setUniformValue("color", _bufferColor);

		auto quadVAO = vao("Quad");

		quadVAO->bind();
		{
			glBindTexture(GL_TEXTURE_2D, selectionFBO->texture());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		quadVAO->release();

		overlayProgram->release();
	}
	*/
}

void Renderer::renderOutline()
{
	/* TODO
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
	*/
}