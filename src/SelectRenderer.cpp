#include "SelectRenderer.h"
#include "ImageViewerWidget.h"

#include <QDebug>

#include <QtMath>

#include <vector>

#include "Shaders.h"

SelectRenderer::SelectRenderer(const std::uint32_t& zIndex, ImageViewerWidget* imageViewerWidget) :
	QuadRenderer(zIndex),
	_imageViewerWidget(imageViewerWidget),
	_texture(),
	_fbo(),
	_color(1.f, 0.6f, 0.f, 0.3f),
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f),
	_pixelSelectionProgram(std::make_unique<QOpenGLShaderProgram>()),
	_outlineProgram(std::make_unique<QOpenGLShaderProgram>())
{
}

void SelectRenderer::init()
{
	QuadRenderer::init();

	const auto stride = 5 * sizeof(GLfloat);

	_pixelSelectionProgram->bind();

	_vao.bind();
	_vbo.bind();

	_pixelSelectionProgram->enableAttributeArray(0);
	_pixelSelectionProgram->enableAttributeArray(1);
	_pixelSelectionProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
	_pixelSelectionProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

	_vao.release();
	_vbo.release();

	_pixelSelectionProgram->release();

	_outlineProgram->bind();

	_outlineProgram->enableAttributeArray(0);
	_outlineProgram->enableAttributeArray(1);
	_outlineProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
	_outlineProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

	_outlineProgram->release();
}

void SelectRenderer::render()
{
	if (!initialized())
		return;

	renderOverlay();

	if (_imageViewerWidget->interactionMode() == InteractionMode::Selection) {
		renderOutline();
	}
}

void SelectRenderer::initializePrograms()
{
	_program->addShaderFromSourceCode(QOpenGLShader::Vertex, overlayVertexShaderSource.c_str());
	_program->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShaderSource.c_str());
	_program->link();

	_pixelSelectionProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, pixelSelectionVertexShaderSource.c_str());
	_pixelSelectionProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, pixelSelectionFragmentShaderSource.c_str());
	_pixelSelectionProgram->link();

	_outlineProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOutlineVertexShaderSource.c_str());
	_outlineProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOutlineFragmentShaderSource.c_str());
	_outlineProgram->link();
}

void SelectRenderer::setImageSize(const QSize& size)
{
	auto createFBO = false;

	if (_fbo.get() == nullptr) {
		createFBO = true;
	}
	else {
		if (size != _fbo->size()) {
			createFBO = true;
		}
	}

	if (createFBO) {
		_fbo = std::make_unique<QOpenGLFramebufferObject>(size.width(), size.height());
	}

	setSize(size);
}

void SelectRenderer::update(const SelectionType& selectionType, const std::vector<QVector3D>& mousePositions)
{
	qDebug() << "Update";

	if (!_fbo->bind())
		return;
	
	glViewport(0, 0, _fbo->width(), _fbo->height());

	QMatrix4x4 transform;

	auto width = _fbo->width();

	transform.ortho(0.0f, _fbo->width(), 0.0f, _fbo->height(), -1.0f, +1.0f);

	_vao.bind();
	{
		if (_pixelSelectionProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _fbo->texture());

			_pixelSelectionProgram->setUniformValue("pixelSelectionTexture", 0);
			_pixelSelectionProgram->setUniformValue("transform", transform);
			_pixelSelectionProgram->setUniformValue("selectionType", static_cast<int>(selectionType));
			_pixelSelectionProgram->setUniformValue("imageSize", static_cast<float>(_fbo->size().width()), static_cast<float>(_fbo->size().height()));

			switch (selectionType)
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= mousePositions.front();
					const auto rectangleBottomRight		= mousePositions.back();
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(_fbo->width()), rectangleTopLeft.y() / static_cast<float>(_fbo->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(_fbo->width()), rectangleBottomRight.y() / static_cast<float>(_fbo->height()));

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

					_pixelSelectionProgram->setUniformValue("rectangleTopLeft", topLeft);
					_pixelSelectionProgram->setUniformValue("rectangleBottomRight", bottomRight);

					break;
				}

				case SelectionType::Brush:
				{
					const auto brushCenter			= mousePositions[mousePositions.size() - 1];
					const auto previousBrushCenter	= mousePositions.size() > 1 ? mousePositions[mousePositions.size() - 2] : brushCenter;

					_pixelSelectionProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					_pixelSelectionProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					_pixelSelectionProgram->setUniformValue("brushRadius", _brushRadius);

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

					_pixelSelectionProgram->setUniformValueArray("points", &points[0], static_cast<std::int32_t>(points.size()));
					_pixelSelectionProgram->setUniformValue("noPoints", static_cast<int>(points.size()));

					break;
				}

				default:
					break;
			}

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_pixelSelectionProgram->release();
		}
	}
	_vao.release();

	_fbo->release();
}

void SelectRenderer::reset()
{
	qDebug() << "Reset";

	if (_fbo->bind()) {
		glViewport(0, 0, _fbo->width(), _fbo->height());
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		_fbo->release();
	}
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
	return std::make_shared<QImage>(_fbo->toImage());
}

bool SelectRenderer::initialized() const
{
	if (_fbo.get() == nullptr)
		return false;

	return _fbo->isValid();
}

void SelectRenderer::renderOverlay()
{
	if (_program->bind()) {
		_program->setUniformValue("overlayTexture", 0);
		_program->setUniformValue("transform", _modelViewProjection);
		_program->setUniformValue("color", _color);

		_vao.bind();
		{
			glBindTexture(GL_TEXTURE_2D, _fbo->texture());
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		_vao.release();

		_program->release();
	}
}

void SelectRenderer::renderOutline()
{
	QMatrix4x4 transform;

	transform.ortho(_imageViewerWidget->rect());

	if (_pixelSelectionProgram->bind()) {
		_pixelSelectionProgram->setUniformValue("transform", _modelViewProjection);
		_pixelSelectionProgram->setUniformValue("color", _color);

		switch (_imageViewerWidget->selectionType())
		{
			case SelectionType::Rectangle:
			{
				if (_imageViewerWidget->selecting()) {
					const auto initialMousePosition = _imageViewerWidget->screenToWorld(_imageViewerWidget->mousePositions().front());
					const auto currentMouseWorldPos = _imageViewerWidget->screenToWorld(_imageViewerWidget->mousePositions().back());

					drawSelectionOutlineRectangle(initialMousePosition, currentMouseWorldPos);
				}

				break;
			}

			case SelectionType::Brush:
			{
				drawSelectionOutlineBrush();
				break;
			}

			case SelectionType::Lasso:
			{
				drawSelectionOutlineLasso();
				break;
			}

			case SelectionType::Polygon:
			{
				drawSelectionOutlinePolygon();
				break;
			}

			default:
				break;
		}

		_pixelSelectionProgram->release();
	}
}

void SelectRenderer::drawSelectionOutlineRectangle(const QVector3D& start, const QVector3D& end)
{
	
	const float vertices[4][3] = {
		start.x(), start.y(), 0.0f,
		end.x(), start.y(), 0.0f,
		end.x(), end.y(), 0.0f,
		start.x(), end.y(), 0.0f
	};

	auto uv = 0.f;

	QVector<float> vertexData;

	vertexData.resize(20);

	for (int j = 0; j < 4; ++j)
	{
		vertexData[j * 5 + 0] = vertices[j][0];
		vertexData[j * 5 + 1] = vertices[j][1];
		vertexData[j * 5 + 2] = vertices[j][2];

		if (j == 0) {
			vertexData[j * 5 + 3] = 0.f;
		}
		else {
			const auto a = QPointF(vertices[j][0], vertices[j][1]) - QPointF(vertices[j - 1][0], vertices[j - 1][1]);

			uv += a.manhattanLength();

			vertexData[j * 5 + 3] = uv;
		}
		
		vertexData[j * 5 + 4] = 0.f;
	}
	/**/

	/*
	_outlineProgram->bind();

	_outlineProgram->enableAttributeArray(0);
	_outlineProgram->enableAttributeArray(1);
	_outlineProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
	_outlineProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

	_outlineProgram->release();
	

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
		}
		_vbo.release();
	}
	_vao.release();
	*/

	/*
	const GLfloat vertexCoordinates[] = {
	  start.x(), start.y(), 0.0f,
	  end.x(), start.y(), 0.0f,
	  end.x(), end.y(), 0.0f,
	  start.x(), end.y(), 0.0f
	};
	*/

	/**/
	const auto vertexLocation = _outlineProgram->attributeLocation("vertex");

	_outlineProgram->setAttributeArray(vertexLocation, vertexData.constData(), 5);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
}

void SelectRenderer::drawSelectionOutlineBrush()
{
	/*
	const auto brushCenter = _imageViewerWidget->screenToWorld(_imageViewerWidget->mousePosition());
	const auto noSegments = 64u;

	qDebug() << brushCenter;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	const auto brushRadius = _brushRadius;// *_zoom;

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta = 2.0f * M_PI * float(s) / float(noSegments);
		const auto x = brushRadius * cosf(theta);
		const auto y = brushRadius * sinf(theta);

		vertexCoordinates[s * 3 + 0] = brushCenter.x() + x;
		vertexCoordinates[s * 3 + 1] = brushCenter.y() + y;
		vertexCoordinates[s * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _program->attributeLocation("vertex");

	_program->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glDrawArrays(GL_LINE_LOOP, 0, noSegments);
	*/
}

void SelectRenderer::drawSelectionOutlineLasso()
{
	/*
	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(_mousePositions.size() * 3);

	for (std::size_t p = 0; p < _mousePositions.size(); p++) {
		const auto mousePosition = _mousePositions[p];

		vertexCoordinates[p * 3 + 0] = mousePosition.x();
		vertexCoordinates[p * 3 + 1] = mousePosition.y();
		vertexCoordinates[p * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glDrawArrays(GL_LINE_LOOP, 0, static_cast<std::int32_t>(_mousePositions.size()));
	*/
}

void SelectRenderer::drawSelectionOutlinePolygon()
{
	/*
	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(_mousePositions.size() * 3);

	for (std::size_t p = 0; p < _mousePositions.size(); p++) {
		const auto mousePosition = _mousePositions[p];

		vertexCoordinates[p * 3 + 0] = mousePosition.x();
		vertexCoordinates[p * 3 + 1] = mousePosition.y();
		vertexCoordinates[p * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glPointSize(4.0f);

	glDrawArrays(GL_LINE_LOOP, 0, static_cast<std::int32_t>(_mousePositions.size()));
	glDrawArrays(GL_POINTS, 0, static_cast<std::int32_t>(_mousePositions.size()));
	*/
}