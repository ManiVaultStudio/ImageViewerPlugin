#include "SelectionRenderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"

SelectionRenderer::SelectionRenderer(const float& depth, ImageViewerWidget* imageViewerWidget) :
	QuadRenderer(depth),
	_imageViewerWidget(imageViewerWidget),
	_bufferColor(255, 153, 0, 77),
	_selectionColor(255, 0, 0, 153),
	_boundsColor(255, 153, 0, 77),
	_outlineColor(255, 153, 0, 77),
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
		_fbos.insert("SelectionBuffer", std::make_shared<QOpenGLFramebufferObject>(size.width(), size.height()));
	}

	setSize(size);
}

void SelectionRenderer::updateSelectionBuffer(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions)
{
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

void SelectionRenderer::setSelectionImage(std::shared_ptr<QImage> image)
{
	auto selectionTexture = std::make_shared<QOpenGLTexture>(*image.get());

	selectionTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["Selection"] = selectionTexture;
}

void SelectionRenderer::setSelectionBounds(const QRect& selectionBounds)
{
	_bounds = selectionBounds;
}

void SelectionRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setAlpha(opacity * 255.0f);
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
	auto overlayProgram = std::make_shared<QOpenGLShaderProgram>();

	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOverlayVertexShaderSource.c_str());
	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOverlayFragmentShaderSource.c_str());
	overlayProgram->link();

	_shaderPrograms.insert("Overlay", overlayProgram);

	auto outlineProgram = std::make_shared<QOpenGLShaderProgram>();

	outlineProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOutlineVertexShaderSource.c_str());
	outlineProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOutlineFragmentShaderSource.c_str());
	outlineProgram->link();

	_shaderPrograms.insert("Outline", outlineProgram);

	auto selectionBufferProgram = std::make_shared<QOpenGLShaderProgram>();

	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBufferVertexShaderSource.c_str());
	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBufferFragmentShaderSource.c_str());
	selectionBufferProgram->link();

	_shaderPrograms.insert("SelectionBuffer", selectionBufferProgram);

	auto selectionProgram = std::make_shared<QOpenGLShaderProgram>();

	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	selectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	selectionProgram->link();

	_shaderPrograms.insert("Selection", selectionProgram);

	auto boundsProgram = std::make_shared<QOpenGLShaderProgram>();

	boundsProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	boundsProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	boundsProgram->link();

	_shaderPrograms.insert("Bounds", boundsProgram);
}

void SelectionRenderer::createTextures()
{
	_textures.insert("Selection", std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D));

	auto boundsStippleImage = QImage(2, 2, QImage::Format::Format_RGBA8888);

	boundsStippleImage.setPixelColor(QPoint(0, 0), _boundsColor);
	boundsStippleImage.setPixelColor(QPoint(1, 0), _boundsColor);
	boundsStippleImage.setPixelColor(QPoint(1, 0), QColor(0, 0, 0, 0));
	boundsStippleImage.setPixelColor(QPoint(1, 1), QColor(0, 0, 0, 0));

	auto boundsStippleTexture = std::make_shared<QOpenGLTexture>(boundsStippleImage);

	boundsStippleTexture->setWrapMode(QOpenGLTexture::Repeat);
	boundsStippleTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["BoundsStipple"] = boundsStippleTexture;

	auto outlineStippleImage = QImage(2, 1, QImage::Format::Format_RGBA8888);

	outlineStippleImage.setPixelColor(QPoint(0, 0), _outlineColor);
	outlineStippleImage.setPixelColor(QPoint(1, 0), QColor(0, 0, 0, 0));

	auto outlineStippleTexture = std::make_shared<QOpenGLTexture>(outlineStippleImage);

	outlineStippleTexture->setWrapMode(QOpenGLTexture::Repeat);
	outlineStippleTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["OutlineStipple"] = outlineStippleTexture;
}

void SelectionRenderer::createVBOs()
{
	auto quadVBO		= std::make_shared<QOpenGLBuffer>();
	auto outlineVBO		= std::make_shared<QOpenGLBuffer>();
	auto boundsVBO		= std::make_shared<QOpenGLBuffer>();

	quadVBO->create();
	outlineVBO->create();
	boundsVBO->create();
	
	_vbos.insert("Quad", quadVBO);
	_vbos.insert("Outline", outlineVBO);
	_vbos.insert("Bounds", boundsVBO);
}

void SelectionRenderer::createVAOs()
{
	auto quadVAO	= std::make_shared<QOpenGLVertexArrayObject>();
	auto outlineVAO	= std::make_shared<QOpenGLVertexArrayObject>();
	auto boundsVAO	= std::make_shared<QOpenGLVertexArrayObject>();

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

		outlineProgram->setUniformValue("transform", _modelViewProjection);
		outlineProgram->setUniformValue("stippleTexture", 0);

		auto* outlineVBO	= vbo("Outline").get();
		auto* outlineVAO	= vao("Outline").get();

		const auto scale = 0.2f;

		auto outlineStippleTexture = texture("OutlineStipple");

		outlineStippleTexture->bind();
		{
			switch (_imageViewerWidget->selectionType())
			{
				case SelectionType::Rectangle:
				{
					if (_imageViewerWidget->selecting() && mousePositions.size() >= 2) {
						const auto start	= mousePositions.front();
						const auto end		= mousePositions.back();

						QVector<QVector2D> points;

						points.append(QVector2D(start.x(), start.y()));
						points.append(QVector2D(end.x(), start.y()));
						points.append(QVector2D(end.x(), end.y()));
						points.append(QVector2D(start.x(), end.y()));

						drawPolyline(points, true, outlineVBO, outlineVAO, true, scale);
					}

					break;
				}

				case SelectionType::Brush:
				{
					const auto brushCenter	= _imageViewerWidget->mousePosition();
					const auto noSegments	= 64u;

					QVector<QVector2D> points;

					std::vector<GLfloat> vertexCoordinates;

					vertexCoordinates.resize(noSegments * 3);

					const auto brushRadius = _brushRadius * _imageViewerWidget->zoom();

					for (std::uint32_t s = 0; s < noSegments; s++) {
						const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
						const auto x		= brushRadius * cosf(theta);
						const auto y		= brushRadius * sinf(theta);

						points.append(QVector2D(brushCenter.x() + x, brushCenter.y() + y));
					}

					drawPolyline(points, true, outlineVBO, outlineVAO, true, scale);

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					if (mousePositions.size() >= 2) {
						QVector<QVector2D> points;

						for (const auto& mousePosition : mousePositions) {
							points.append(QVector2D(mousePosition.x(), mousePosition.y()));
						}

						drawPolyline(points, true, outlineVBO, outlineVAO, false, scale);
					
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
			glLineWidth(30);

			drawPolyline(points, false, vbo("Bounds").get(), vao("Bounds").get(), true, 1.0f);
		}
		boundsStippleTexture->release();
	}
}

void SelectionRenderer::drawPolyline(QVector<QVector2D>& points, const bool& screenCoordinates, QOpenGLBuffer* vbo, QOpenGLVertexArrayObject* vao, const bool& closed /*= true*/, const float& scale /*= 1.f*/)
{
	if (closed) {
		points.append(points.first());
	}

	QVector<float> vertexData;

	vertexData.resize(points.size() * 5);

	auto u = 0.f;

	for (int j = 0; j < points.size(); ++j)
	{
		const auto worldPoint = screenCoordinates ? _imageViewerWidget->screenToWorld(QPoint(points[j][0], points[j][1])) : QVector3D(points[j][0], points[j][1], 0.f);

		vertexData[j * 5 + 0] = worldPoint.x();
		vertexData[j * 5 + 1] = worldPoint.y();
		vertexData[j * 5 + 2] = 0.f;

		if (j == 0) {
			vertexData[j * 5 + 3] = 0.5f;
		}
		else {
			const auto a = QPointF(points[j][0], points[j][1]) - QPointF(points[j - 1][0], points[j - 1][1]);

			u += scale * a.manhattanLength();

			vertexData[j * 5 + 3] = u;
		}

		vertexData[j * 5 + 4] = 0.f;
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
			glLineWidth(20);
			glDrawArrays(GL_LINE_STRIP, 0, points.size());
		}
		vbo->release();
	}
	vao->release();
}