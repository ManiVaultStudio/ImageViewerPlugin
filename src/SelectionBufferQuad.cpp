#include "SelectionBufferQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "Shaders.h"

SelectionBufferQuad::SelectionBufferQuad(const QString& name /*= "SelectionBufferQuad"*/, const float& z /*= 0.f*/) :
	Quad(name, z),
	_size(),
	_color(255, 153, 0, 40),
	_brushRadius(10.0f),
	_brushRadiusDelta(1.f)
{
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

	setRectangle(QRectF(0, 0, _size.width(), _size.height()));

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

float SelectionBufferQuad::brushRadius() const
{
	return _brushRadius;
}

void SelectionBufferQuad::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 100000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius to" << QString::number(_brushRadius, 'f', 1) << "for" << _name;

	emit brushRadiusChanged(_brushRadius);
}

float SelectionBufferQuad::brushRadiusDelta() const
{
	return _brushRadiusDelta;
}

void SelectionBufferQuad::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.1f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = boundBrushRadiusDelta;

	qDebug() << "Set brush radius delta" << _brushRadiusDelta << "for" << _name;

	emit brushRadiusDeltaChanged(_brushRadiusDelta);
}

void SelectionBufferQuad::brushSizeIncrease()
{
	setBrushRadius(_brushRadius + _brushRadiusDelta);
}

void SelectionBufferQuad::brushSizeDecrease()
{
	setBrushRadius(_brushRadius - _brushRadiusDelta);
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

void SelectionBufferQuad::update(std::vector<QVector3D> mousePositions, const SelectionType& selectionType)
{
	qDebug() << "Set mouse position for" << _name;

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
			selectionBufferProgram->setUniformValue("selectionType", static_cast<int>(selectionType));

			auto imageWidth		= static_cast<float>(selectionBufferFBO->size().width());
			auto imageHeight	= static_cast<float>(selectionBufferFBO->size().height());

			selectionBufferProgram->setUniformValue("imageSize", imageWidth, imageHeight);

			switch (selectionType)
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= mousePositions.front();
					const auto rectangleBottomRight		= mousePositions.back();
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(selectionBufferFBO->width()), rectangleTopLeft.y() / static_cast<float>(selectionBufferFBO->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(selectionBufferFBO->width()), rectangleBottomRight.y() / static_cast<float>(selectionBufferFBO->height()));

					auto rectangleUV	= std::make_pair(rectangleTopLeftUV, rectangleBottomRightUV);
					auto topLeft		= QVector2D(rectangleTopLeftUV.x(), rectangleTopLeftUV.y());
					auto bottomRight	= QVector2D(rectangleBottomRightUV.x(), rectangleBottomRightUV.y());

					qDebug() << rectangleTopLeftUV << rectangleBottomRightUV;

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

	selectionBufferFBO->release();
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

QSharedPointer<QImage> SelectionBufferQuad::selectionImage() const
{
	auto selectionFBO = fbo("SelectionBuffer");

	return QSharedPointer<QImage>::create(selectionFBO->toImage());
}
