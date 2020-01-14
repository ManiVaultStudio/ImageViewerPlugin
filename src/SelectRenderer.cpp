#include "SelectRenderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"

SelectRenderer::SelectRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget) :
	QuadRenderer(zIndex),
	_imageViewerWidget(imageViewerWidget),
	_color(1.f, 0.6f, 0.f, 0.3f),
	_selectionColor(1.f, 0.f, 0.f, 0.6f),
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f),
	_outlineColor(1.f, 0.6f, 0.f, 1.0f),
	_outlineVBO(),
	_outlineVAO()
{
}

void SelectRenderer::init()
{
	QuadRenderer::init();

	const auto stride = 5 * sizeof(GLfloat);

	auto outlineProgram = shaderProgram("Outline");

	_outlineVAO.create();
	_outlineVBO.create();

	if (outlineProgram->bind()) {
		_outlineVAO.bind();
		_outlineVBO.bind();

		outlineProgram->enableAttributeArray(0);
		outlineProgram->enableAttributeArray(1);
		outlineProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		outlineProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		outlineProgram->release();

		_outlineVBO.release();
		_outlineVAO.release();
	}

	auto selectionBufferProgram = shaderProgram("SelectionBuffer");

	if (selectionBufferProgram->bind()) {
		auto quadVAO = vao("Quad");
		auto quadVBO = vbo("Quad");

		quadVAO->bind();
		quadVBO->bind();

		selectionBufferProgram->enableAttributeArray(0);
		selectionBufferProgram->enableAttributeArray(1);
		selectionBufferProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		selectionBufferProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVAO->release();
		quadVBO->release();

		selectionBufferProgram->release();
	}

	auto selectionProgram = shaderProgram("Selection");

	if (selectionProgram->bind()) {
		auto quadVAO = vao("Quad");
		auto quadVBO = vbo("Quad");

		quadVAO->bind();
		quadVBO->bind();

		selectionProgram->enableAttributeArray(0);
		selectionProgram->enableAttributeArray(1);
		selectionProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
		selectionProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

		quadVAO->release();
		quadVBO->release();

		selectionProgram->release();
	}
}

void SelectRenderer::render()
{
	if (!isInitialized())
		return;

	renderOverlay();
	renderSelection();

	if (_imageViewerWidget->interactionMode() == InteractionMode::Selection) {
		renderOutline();
	}
}

void SelectRenderer::setImageSize(const QSize& size)
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

void SelectRenderer::update(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions)
{
	qDebug() << "Update";

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

void SelectRenderer::resetSelectionBuffer()
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

void SelectRenderer::setSelectionImage(std::shared_ptr<QImage> image)
{
	auto selectionTexture = std::make_shared<QOpenGLTexture>(*image.get());

	selectionTexture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);

	_textures["Selection"] = selectionTexture;
}

void SelectRenderer::setOpacity(const float& opacity)
{
	_selectionColor.setW(opacity);
}

float SelectRenderer::brushRadius() const
{
	return _brushRadius;
}

void SelectRenderer::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius" << brushRadius;
}

float SelectRenderer::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectRenderer::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;
}

void SelectRenderer::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectRenderer::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
}

std::shared_ptr<QImage> SelectRenderer::selectionImage() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	return std::make_shared<QImage>(selectionFBO->toImage());
}

bool SelectRenderer::isInitialized() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	if (selectionFBO.get() == nullptr)
		return false;

	return selectionFBO->isValid();
}

void SelectRenderer::createShaderPrograms()
{
	auto overlayProgram = std::make_shared<QOpenGLShaderProgram>();

	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, overlayVertexShaderSource.c_str());
	overlayProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShaderSource.c_str());
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
}

void SelectRenderer::createTextures()
{
	_textures.insert("Selection", std::make_shared<QOpenGLTexture>(QOpenGLTexture::Target2D));
}

void SelectRenderer::createVBOs()
{
	auto quadVBO = std::make_shared<QOpenGLBuffer>();

	quadVBO->create();
	
	_vbos.insert("Quad", quadVBO);
}

void SelectRenderer::createVAOs()
{
	auto quadVAO = std::make_shared<QOpenGLVertexArrayObject>();

	quadVAO->create();

	_vaos.insert("Quad", quadVAO);
}

void SelectRenderer::renderOverlay()
{
	auto selectionFBO = fbo("SelectionBuffer");

	auto overlayProgram = shaderProgram("Overlay");

	if (overlayProgram->bind()) {
		overlayProgram->setUniformValue("overlayTexture", 0);
		overlayProgram->setUniformValue("transform", _modelViewProjection);
		overlayProgram->setUniformValue("color", _color);

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

void SelectRenderer::renderSelection()
{
	auto& selectionTexture = texture("Selection");

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

void SelectRenderer::renderOutline()
{
	auto outlineProgram = shaderProgram("Outline");

	if (outlineProgram->bind()) {
		const auto mousePositions = _imageViewerWidget->mousePositions();

		QMatrix4x4 transform;

		transform.ortho(_imageViewerWidget->rect());

		outlineProgram->setUniformValue("transform", _modelViewProjection);
		outlineProgram->setUniformValue("color", _outlineColor);

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

					drawPolyline(points);
				}

				break;
			}

			case SelectionType::Brush:
			{
				if (mousePositions.size() >= 1) {
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

					drawPolyline(points);
				}

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

					drawPolyline(points);
				}

				break;
			}

			default:
				break;
		}

		outlineProgram->release();
	}
}

void SelectRenderer::drawPolyline(const QVector<QVector2D>& points)
{
	auto uv = 0.f;

	QVector<float> vertexData;

	vertexData.resize(points.size() * 5);

	for (int j = 0; j < points.size(); ++j)
	{
		const auto worldPoint = _imageViewerWidget->screenToWorld(QPoint(points[j][0], points[j][1]));

		vertexData[j * 5 + 0] = worldPoint.x();
		vertexData[j * 5 + 1] = worldPoint.y();
		vertexData[j * 5 + 2] = 0.f;

		if (j == 0) {
			vertexData[j * 5 + 3] = 0.f;
		}
		else {
			const auto a = QPointF(points[j][0], points[j][1]) - QPointF(points[j - 1][0], points[j - 1][1]);

			uv += a.manhattanLength();

			vertexData[j * 5 + 3] = uv;
		}
		
		vertexData[j * 5 + 4] = 0.f;
	}
	
	_outlineVBO.bind();
	_outlineVBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);
	_outlineVBO.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
	_outlineVBO.release();

	_outlineVAO.bind();
	_outlineVBO.bind();

	glDrawArrays(GL_LINE_LOOP, 0, points.size());

	_outlineVAO.release();
	_outlineVBO.release();
}