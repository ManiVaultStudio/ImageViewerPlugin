#include "SelectionBufferQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QMouseEvent>
#include <QDebug>

#include "Renderer.h"
#include "Shaders.h"

SelectionBufferQuad::SelectionBufferQuad(Renderer* renderer, const QString& name, const float& z /*= 0.f*/) :
	Quad(renderer, name, z),
	_size(),
	_color(255, 153, 0, 40),
	_mousePositions()
{
	_mouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);
}

void SelectionBufferQuad::render()
{
	if (!canRender())
		return;

	//qDebug() << "Render" << _name << "shape";

	if (bindShaderProgram("Quad")) {
		glBindTexture(GL_TEXTURE_2D, fbo("SelectionBuffer")->texture());

		vao("Quad")->bind();
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		vao("Quad")->release();

		shaderProgram("Quad")->release();
	}
}

QSize SelectionBufferQuad::size() const
{
	return _size;
}

void SelectionBufferQuad::setSize(const QSize& size)
{
	if (size == _size)
		return;

	_size = size;

	qDebug() << "Set size to" << _size << "for" << _name;

	if (!_fbos.contains("SelectionBuffer")) {
		_fbos.insert("SelectionBuffer", QSharedPointer<QOpenGLFramebufferObject>::create(_size.width(), _size.height()));
	}
	else {
		_fbos["SelectionBuffer"] = QSharedPointer<QOpenGLFramebufferObject>::create(_size.width(), _size.height());
	}

	setRectangle(QRectF(QPointF(), QSizeF(static_cast<float>(_size.width()), static_cast<float>(_size.height()))));

	emit sizeChanged(_size);
}

QColor SelectionBufferQuad::color() const
{
	return _color;
}

void SelectionBufferQuad::setColor(const QColor& color)
{
	if (color == _color)
		return;

	_color = color;

	qDebug() << "Set color to" << _color << "for" << _name;

	emit colorChanged(_color);
}

float SelectionBufferQuad::opacity() const
{
	return _color.alphaF();
}

void SelectionBufferQuad::setOpacity(const float& opacity)
{
	if (opacity == _color.alphaF())
		return;

	_color.setAlphaF(opacity);

	qDebug() << "Set opacity to" << _color.alphaF() << "for" << _name;

	emit opacityChanged(_color.alphaF());
}

void SelectionBufferQuad::update()
{
	Quad::update();

	auto selectionBufferFBO = fbo("SelectionBuffer");

	if (!selectionBufferFBO->bind())
		return;

	glViewport(0, 0, selectionBufferFBO->width(), selectionBufferFBO->height());

	QMatrix4x4 transform;

	auto width = selectionBufferFBO->width();

	transform.ortho(0.0f, selectionBufferFBO->width(), 0.0f, selectionBufferFBO->height(), -1.0f, +1.0f);

	auto quadVAO = vao("Quad");

	quadVAO->bind();
	{
		auto selectionBufferProgram = shaderProgram("SelectionBuffer");

		if (selectionBufferProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, selectionBufferFBO->texture());

			selectionBufferProgram->setUniformValue("pixelSelectionTexture", 0);
			selectionBufferProgram->setUniformValue("transform", transform);
			selectionBufferProgram->setUniformValue("selectionType", static_cast<int>(_renderer->selectionType()));

			auto imageWidth = static_cast<float>(selectionBufferFBO->size().width());
			auto imageHeight = static_cast<float>(selectionBufferFBO->size().height());

			selectionBufferProgram->setUniformValue("imageSize", imageWidth, imageHeight);

			switch (_renderer->selectionType())
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft = _mousePositions.front();
					const auto rectangleBottomRight = _mousePositions.back();
					const auto rectangleTopLeftUV = QVector2D(rectangleTopLeft.x() / static_cast<float>(selectionBufferFBO->width()), rectangleTopLeft.y() / static_cast<float>(selectionBufferFBO->height()));
					const auto rectangleBottomRightUV = QVector2D(rectangleBottomRight.x() / static_cast<float>(selectionBufferFBO->width()), rectangleBottomRight.y() / static_cast<float>(selectionBufferFBO->height()));

					auto rectangleUV = std::make_pair(rectangleTopLeftUV, rectangleBottomRightUV);
					auto topLeft = QVector2D(rectangleTopLeftUV.x(), rectangleTopLeftUV.y());
					auto bottomRight = QVector2D(rectangleBottomRightUV.x(), rectangleBottomRightUV.y());

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
					const auto brushCenter = _mousePositions[_mousePositions.size() - 1];
					const auto previousBrushCenter = _mousePositions.size() > 1 ? _mousePositions[_mousePositions.size() - 2] : brushCenter;

					selectionBufferProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					selectionBufferProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					selectionBufferProgram->setUniformValue("brushRadius", _renderer->brushRadius());

					break;
				}

				case SelectionType::Lasso:
				case SelectionType::Polygon:
				{
					QList<QVector2D> points;

					points.reserve(static_cast<std::int32_t>(_mousePositions.size()));

					for (const auto& mousePosition : _mousePositions) {
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

	selectionBufferFBO->release();

	emit changed(this);
}

void SelectionBufferQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	auto quadProgram = shaderProgram("Quad");

	quadProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionOverlayVertexShaderSource.c_str());
	quadProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionOverlayFragmentShaderSource.c_str());
	quadProgram->link();

	addShaderProgram("SelectionBuffer", QSharedPointer<QOpenGLShaderProgram>::create());

	auto selectionBufferProgram = shaderProgram("SelectionBuffer");

	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBufferVertexShaderSource.c_str());
	selectionBufferProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBufferFragmentShaderSource.c_str());
	selectionBufferProgram->link();
}

void SelectionBufferQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("overlayTexture", 0);
		quadProgram->setUniformValue("color", _color);
	}
}

void SelectionBufferQuad::reset()
{
	qDebug() << "Reset" << _name;
	
	auto selectionFBO = fbo("SelectionBuffer");

	if (!selectionFBO->bind())
		return;

	glViewport(0, 0, selectionFBO->width(), selectionFBO->height());
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	selectionFBO->release();
}

QSharedPointer<QImage> SelectionBufferQuad::selectionBufferImage() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	return QSharedPointer<QImage>::create(selectionFBO->toImage());
}

void SelectionBufferQuad::onMousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event for" << _name;

	if (_renderer->selectionType() != SelectionType::Polygon) {
		_mousePositions.clear();
	}

	if (mouseEvent->button() == Qt::LeftButton) {
		_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), _renderer->projectionMatrix(), mouseEvent->pos()));
	}

	update();
}

void SelectionBufferQuad::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event for" << _name;

	update();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_renderer->selectionType() != SelectionType::Polygon) {
				
				deactivate();
			}

			break;
		}

		case Qt::RightButton:
		{
			if (_renderer->selectionType() == SelectionType::Polygon) {
				deactivate();
			}

			break;
		}

		default:
			break;
	}
}

void SelectionBufferQuad::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event for" << _name;

	if (_renderer->selectionType() != SelectionType::Polygon) {
		_mousePositions.push_back(_renderer->screenToWorld(modelViewMatrix(), _renderer->projectionMatrix(), QPointF(mouseEvent->pos())));
		update();
	}
}

void SelectionBufferQuad::activate()
{
	if (isActive())
		return;

	Quad::activate();

	_mousePositions.clear();
}

void SelectionBufferQuad::deactivate()
{
	if (!isActive())
		return;

	emit selectionEnded();

	Quad::deactivate();

	_mousePositions.clear();
}

QVector<QVector3D> SelectionBufferQuad::mousePositions() const
{
	return _mousePositions;
}