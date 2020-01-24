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
	_color(255, 0, 0, 200)
{
}

void SelectionBufferQuad::render()
{
	if (!canRender())
		return;

	auto quadFbo = fbo("Quad");

	if (!quadFbo->isValid())
		return;

	Shape::render();

	//qDebug() << "Render" << _name << "shape";

	if (isTextured()) {
		glBindTexture(GL_TEXTURE_2D, quadFbo->texture());
	}

	if (bindShaderProgram("Quad")) {
		vao("Quad")->bind();
		{
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
		vao("Quad")->release();

		shaderProgram("Quad")->release();
	}

	/*
	if (isTextured()) {
	}
	*/
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

	qDebug() << "Set size to" << size << "for" << _name;

	if (_fbos.contains("Quad")) {
		_fbos.insert("Quad", QSharedPointer<QOpenGLFramebufferObject>::create(_size.width(), _size.height()));
	}

	emit sizeChanged(_size);
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

	qDebug() << "Set opacity" << _color.alphaF();

	emit opacityChanged(_color.alphaF());
}

void SelectionBufferQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBufferVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBufferFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void SelectionBufferQuad::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	addTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));

	texture("Quad")->setWrapMode(QOpenGLTexture::Repeat);
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture("Quad")->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void SelectionBufferQuad::addFBOs()
{
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

void SelectionBufferQuad::update()
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

void SelectionBufferQuad::reset()
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