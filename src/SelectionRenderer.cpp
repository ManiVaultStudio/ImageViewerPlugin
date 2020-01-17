#include "SelectionRenderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"

SelectionRenderer::SelectionRenderer(const float& depth, ImageViewerWidget* imageViewerWidget) :
	QuadRenderer(depth),
	_imageViewerWidget(imageViewerWidget),
	_bufferColor(255, 153, 0, 70),
	_selectionColor(255, 0, 0, 153),
	_boundsColor(255, 153, 0, 70),
	_boundsLineWidth(2.0f),
	_outlineColor(255, 153, 0, 255),
	_outlineLineWidth(3.0f),
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f),
	_bounds()
{
}

void SelectionRenderer::init()
{
	QuadRenderer::init();

	const auto stride = 5 * sizeof(GLfloat);

	auto quadVAO = vao("Quad");
	auto quadVBO = vbo("Quad");

	auto selectionBufferProgram = shaderProgram("SelectionBuffer");

	if (selectionBufferProgram->bind()) {
		selectionBufferProgram->enableAttributeArray(0);
		selectionBufferProgram->enableAttributeArray(1);
		selectionBufferProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		selectionBufferProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVBO->release();
		quadVAO->release();
		selectionBufferProgram->release();
	}

	auto selectionProgram = shaderProgram("Selection");

	if (selectionProgram->bind()) {
		quadVAO->bind();
		quadVBO->bind();

		selectionProgram->enableAttributeArray(0);
		selectionProgram->enableAttributeArray(1);
		selectionProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		selectionProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVBO->release();
		quadVAO->release();
		selectionProgram->release();
	}

	auto boundsProgram	= shaderProgram("Bounds");
	auto boundsVAO		= vao("Bounds");
	auto boundsVBO		= vbo("Bounds");

	if (boundsProgram->bind()) {
		boundsVAO->bind();
		boundsVBO->bind();

		boundsProgram->enableAttributeArray(0);
		boundsProgram->enableAttributeArray(1);
		boundsProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		boundsProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		
		boundsVBO->release();
		boundsVAO->release();
		boundsProgram->release();
	}

	auto outlineProgram	= shaderProgram("Outline");
	auto outlineVBO		= vbo("Outline");
	auto outlineVAO		= vao("Outline");

	if (outlineProgram->bind()) {
		outlineVAO->bind();
		outlineVBO->bind();

		outlineProgram->enableAttributeArray(0);
		outlineProgram->enableAttributeArray(1);
		outlineProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		outlineProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		outlineVBO->release();
		outlineVAO->release();
		outlineProgram->release();
	}
}

void SelectionRenderer::render()
{
	if (!isInitialized())
		return;

	renderOverlay();
	renderSelection();
	renderOutline();
	renderBounds();
}

void SelectionRenderer::setImageSize(const QSize& size)
{
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
}

void SelectionRenderer::updateSelectionBuffer(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions)
{
	//qDebug() << "Update selection buffer";

	auto selectionFBO = fbo("SelectionBuffer");

	if (!selectionFBO->bind())
		return;
	
	glViewport(0, 0, selectionFBO->width(), selectionFBO->height());

	QMatrix4x4 transform;

	auto width = selectionFBO->width();

	transform.ortho(0.0f, selectionFBO->width(), 0.0f, selectionFBO->height(), -1.0f, +1.0f);

	auto quadVAO = vao("Quad");

	quadVAO->bind();
	{
		auto selectionBufferProgram = shaderProgram("SelectionBuffer");

		if (selectionBufferProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, selectionFBO->texture());

			selectionBufferProgram->setUniformValue("pixelSelectionTexture", 0);
			selectionBufferProgram->setUniformValue("transform", transform);
			selectionBufferProgram->setUniformValue("selectionType", static_cast<int>(selectionType));
			selectionBufferProgram->setUniformValue("imageSize", static_cast<float>(selectionFBO->size().width()), static_cast<float>(selectionFBO->size().height()));

			switch (selectionType)
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
}

void SelectionRenderer::resetSelectionBuffer()
{
	qDebug() << "Reset";

	auto selectionFBO = fbo("SelectionBuffer");
	
	if (!selectionFBO->bind())
		return;

	glViewport(0, 0, selectionFBO->width(), selectionFBO->height());
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	selectionFBO->release();
}

void SelectionRenderer::setSelectionImage(std::shared_ptr<QImage> selectionImage)
{
	auto selectionTexture = QSharedPointer<QOpenGLTexture>::create(*selectionImage.get());

	selectionTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["Selection"] = selectionTexture;
}

void SelectionRenderer::setSelectionBounds(const QRect& selectionBounds)
{
	_bounds = selectionBounds;
}

float SelectionRenderer::selectionOpacity() const
{
	return _selectionColor.alphaF();
}

void SelectionRenderer::setSelectionOpacity(const float& selectionOpacity)
{
	if (selectionOpacity == _selectionColor.alphaF())
		return;

	_selectionColor.setAlphaF(selectionOpacity);

	qDebug() << "Set selection opacity" << selectionOpacity;

	emit selectionOpacityChanged(_selectionColor.alphaF());
}

float SelectionRenderer::brushRadius() const
{
	return _brushRadius;
}

void SelectionRenderer::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius" << brushRadius;
}

float SelectionRenderer::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectionRenderer::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;
}

void SelectionRenderer::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectionRenderer::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

std::shared_ptr<QImage> SelectionRenderer::selectionImage() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	return std::make_shared<QImage>(selectionFBO->toImage());
}

bool SelectionRenderer::isInitialized() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	if (selectionFBO.get() == nullptr)
		return false;

	return selectionFBO->isValid();
}

void SelectionRenderer::createShaderPrograms()
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

	auto boundsProgram = QSharedPointer<QOpenGLShaderProgram>::create();

	boundsProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	boundsProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	boundsProgram->link();

	_shaderPrograms.insert("Bounds", boundsProgram);
}

void SelectionRenderer::createTextures()
{
	_textures.insert("Selection", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));

	auto boundsStippleImage = QImage(2, 1, QImage::Format::Format_RGBA8888);

	boundsStippleImage.setPixelColor(QPoint(0, 0), _boundsColor);
	boundsStippleImage.setPixelColor(QPoint(1, 0), QColor(0, 0, 0, 0));

	auto boundsStippleTexture = QSharedPointer<QOpenGLTexture>::create(boundsStippleImage);

	boundsStippleTexture->setWrapMode(QOpenGLTexture::Repeat);
	boundsStippleTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["BoundsStipple"] = boundsStippleTexture;

	auto outlineStippleImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	auto fade = _outlineColor;

	fade.setAlpha(0);

	outlineStippleImage.setPixelColor(QPoint(0, 0), _outlineColor);

	auto outlineStippleTexture = QSharedPointer<QOpenGLTexture>::create(outlineStippleImage);

	outlineStippleTexture->setWrapMode(QOpenGLTexture::Repeat);
	outlineStippleTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_textures["OutlineStipple"] = outlineStippleTexture;
}

void SelectionRenderer::createVBOs()
{
	auto quadVBO		= QSharedPointer<QOpenGLBuffer>::create();
	auto outlineVBO		= QSharedPointer<QOpenGLBuffer>::create();
	auto boundsVBO		= QSharedPointer<QOpenGLBuffer>::create();

	quadVBO->create();
	outlineVBO->create();
	boundsVBO->create();
	
	_vbos.insert("Quad", quadVBO);
	_vbos.insert("Outline", outlineVBO);
	_vbos.insert("Bounds", boundsVBO);
}

void SelectionRenderer::createVAOs()
{
	auto quadVAO	= QSharedPointer<QOpenGLVertexArrayObject>::create();
	auto outlineVAO	= QSharedPointer<QOpenGLVertexArrayObject>::create();
	auto boundsVAO	= QSharedPointer<QOpenGLVertexArrayObject>::create();

	quadVAO->create();
	outlineVAO->create();
	boundsVAO->create();

	_vaos.insert("Quad", quadVAO);
	_vaos.insert("Outline", outlineVAO);
	_vaos.insert("Bounds", boundsVAO);
}

void SelectionRenderer::renderOverlay()
{
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
}

void SelectionRenderer::renderSelection()
{
	auto selectionTexture = texture("Selection");

	if (selectionTexture.get() == nullptr || !selectionTexture->isCreated())
		return;

	auto selectionProgram = shaderProgram("Selection");

	if (selectionProgram->bind()) {
		selectionProgram->setUniformValue("selectionTexture", 0);
		selectionProgram->setUniformValue("transform", _modelViewProjection);
		selectionProgram->setUniformValue("color", _selectionColor);

		selectionTexture->bind();
		{
			QuadRenderer::render();
		}
		selectionTexture->release();

		selectionProgram->release();
	}
}

void SelectionRenderer::renderOutline()
{
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

void SelectionRenderer::renderBounds()
{
	auto boundsProgram = shaderProgram("Bounds");

	if (boundsProgram->bind()) {
		boundsProgram->setUniformValue("stippleTexture", 0);
		boundsProgram->setUniformValue("transform", _modelViewProjection);
		boundsProgram->setUniformValue("color", _outlineColor);

		const auto p0 = _bounds.topLeft();
		const auto p1 = _bounds.bottomRight();

		QVector<QVector2D> points;

		points.append(QVector2D(p0.x(), p0.y()));
		points.append(QVector2D(p1.x(), p0.y()));
		points.append(QVector2D(p1.x(), p1.y()));
		points.append(QVector2D(p0.x(), p1.y()));

		auto boundsStippleTexture = texture("BoundsStipple");

		boundsStippleTexture->bind();
		{
			drawPolyline(points, vbo("Bounds").get(), vao("Bounds").get(), true, _boundsLineWidth, 0.1f);
		}
		boundsStippleTexture->release();
	}
}

void SelectionRenderer::drawPolyline(QVector<QVector2D> points, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, const bool& closed /*= true*/, const float& lineWidth /*= 1.f*/, const float& textureScale /*= 0.05f*/)
{
	if (closed && points.size() > 2) {
		points.append(points.first());
	}
	
	QVector<float> vertexData;

	const auto noPoints = points.size();

	vertexData.reserve(noPoints * 5);

	const auto pWorld0			= _imageViewerWidget->screenToWorld(QPointF(0.0f, 0.0f));
	const auto pWorld1			= _imageViewerWidget->screenToWorld(QPointF(lineWidth, 0.0f));
	const auto worldLineWidth	= (pWorld1 - pWorld0).length();
	const auto halfLineWidth	= 0.5f * worldLineWidth;

	QVector<QPair<QVector2D, QVector2D>> coordinates;

	auto halfAngleVector = [](const QVector2D& v0, const QVector2D& v1) {
		if (std::abs(QVector2D::dotProduct(v0, v1)) == 1.0f)
			return QVector2D(-v0.y(), v0.x()).normalized();

		return (v0 + v1).normalized();
	};

	auto outsideVectorAtPoint = [&points, &noPoints, &closed, &halfAngleVector, &halfLineWidth](const std::uint32_t& id, const QVector2D& direction) {
		if (id == 0) {
			if (closed) {
				const auto p			= points[0];
				const auto v0			= (points[noPoints - 2] - p).normalized();
				const auto v1			= (points[1] - p).normalized();
				const auto vHalfAngle	= halfAngleVector(v0, v1);
				const auto vOutside		= halfLineWidth * vHalfAngle * (1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

				if (QVector2D::dotProduct(vOutside, direction) < 0)
					return -vOutside;

				return vOutside;
			}
			else {
				const auto p = points[1];
				const auto a = (points[0] - p).normalized();
				const auto b = (points[2] - p).normalized();
				const auto v = (points[1] - points[0]).normalized();
				const auto ha = halfAngleVector(a, b);
				const auto vp = halfLineWidth * QVector2D(-v.y(), v.x());

				if (QVector2D::dotProduct(vp, ha) < 0)
					return -vp;

				return vp;
			}
		}

		if (id == noPoints - 1) {
			if (closed) {
				const auto p			= points[0];
				const auto v0			= (points[noPoints - 2] - p).normalized();
				const auto v1			= (points[1] - p).normalized();
				const auto vHalfAngle	= halfAngleVector(v0, v1);
				const auto vOutside		= halfLineWidth * vHalfAngle * (1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

				if (QVector2D::dotProduct(vOutside, direction) < 0)
					return -vOutside;

				return vOutside;
			}
			else {
				const auto p = points[noPoints - 1];
				const auto a = (points[noPoints - 3] - p).normalized();
				const auto b = (points[noPoints - 2] - p).normalized();
				const auto v = (points[noPoints - 2] - points[noPoints - 1]).normalized();
				const auto ha = halfAngleVector(a, b);
				const auto vp = halfLineWidth * QVector2D(-v.y(), v.x());

				if (QVector2D::dotProduct(vp, ha) < 0)
					return -vp;

				return vp;
			}
		}

		const auto p			= points[id];
		const auto v0			= (points[id - 1] - p).normalized();
		const auto v1			= (points[id + 1] - p).normalized();
		const auto vHalfAngle	= halfAngleVector(v0, v1);
		const auto vOutside		= halfLineWidth * vHalfAngle *(1.0f / std::abs(QVector2D::dotProduct(vHalfAngle, QVector2D(-v0.y(), v0.x()).normalized())));

		//qDebug() << v0 << v1 << vHalfAngle;
		if (QVector2D::dotProduct(vHalfAngle, direction) < 0)
			return -vOutside;

		return vOutside;
	};

	auto outsidePoint = [&points, &noPoints, &closed, &outsideVectorAtPoint, &halfLineWidth](const std::uint32_t& id, const QVector2D& direction) {
		if (noPoints == 0)
			return QVector2D(0.0f, 0.0f);

		if (noPoints == 1)
			return QVector2D(0.0f, 0.0f);

		if (noPoints == 2) {
			const auto v = (points[1] - points[0]).normalized();
			return halfLineWidth * QVector2D(-v.y(), v.x());
		}

		if (id >= 0 && id < noPoints) {
			return outsideVectorAtPoint(id, direction);
		}

		return QVector2D(0.0f, 0.0f);
	};

	const auto v = (points[1] - points[0]).normalized();

	auto direction = QVector2D(-v.y(), v.x()).normalized();

	for (int j = 0; j < points.size(); ++j)
	{
		const auto pOutside	= outsidePoint(j, direction);
		const auto vInner	= points[j] - pOutside;
		const auto vOuter	= points[j] + pOutside;

		coordinates.append(QPair(vInner, vOuter));

		if (j < points.size() - 1) {
			const auto p1		= points[j + 1];
			const auto p0		= points[j];
			const auto vPar		= (p1 - p0).normalized();
			const auto vPerp	= QVector2D(-vPar.y(), vPar.x()).normalized();
			const auto dot		= QVector2D::dotProduct(pOutside, vPerp);

			direction = vPerp;

			if (dot < 0)
				direction *= -1.0f;
		}
	}

	auto addVertex = [&vertexData](const float& x, const float& y, const float& u, const float& v) {
		vertexData.append(x);
		vertexData.append(y);
		vertexData.append(0.0f);
		vertexData.append(u);
		vertexData.append(v);
	};

	for (const auto& coordinate : coordinates) {
		const auto inner = coordinate.first;
		const auto outer = coordinate.second;

		addVertex(inner.x(), inner.y(), 0.0f, 0.0f);
		addVertex(outer.x(), outer.y(), 0.0f, 1.0f);
	}

	vbo->bind();
	{
		vbo->setUsagePattern(QOpenGLBuffer::DynamicDraw);
		vbo->allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
		vbo->release();
	}

	vao->bind();
	{
		vbo->bind();
		{
			glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexData.size() / 5);
		}
		vbo->release();
	}
	vao->release();
}