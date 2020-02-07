#include "SelectionImageProp.h"
#include "QuadShape.h"
#include "Actor.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

const std::string vertexShaderSource =
#include "SelectionImageVertex.glsl"
;

const std::string fragmentShaderSource =
#include "SelectionImageFragment.glsl"
;

SelectionImageProp::SelectionImageProp(Actor* actor, const QString& name) :
	Prop(actor, name)
{
	_color = QColor(255, 0, 0, 255);

	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addTexture("Quad", QOpenGLTexture::Target2D);
}

void SelectionImageProp::initialize()
{
	try
	{
		Prop::initialize();

		const auto shaderProgram = shaderProgramByName("Quad");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
			throw std::exception("Unable to compile quad vertex shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
			throw std::exception("Unable to compile quad fragment shader");

		if (!shaderProgram->link())
			throw std::exception("Unable to link quad shader program");

		const auto stride = 5 * sizeof(GLfloat);

		auto shape = shapeByName<QuadShape>("Quad");

		if (shaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
			shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
			shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
			shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

			shape->vao().release();
			shape->vbo().release();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		const auto texture = textureByName("Quad");

		texture->setWrapMode(QOpenGLTexture::Repeat);
		texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

		_initialized = true;
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "initialization failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "initialization failed due to unhandled exception";
	}
}

void SelectionImageProp::render()
{
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shape = shapeByName<QuadShape>("Quad");
		const auto shaderProgram = shaderProgramByName("Quad");
		const auto texture = textureByName("Quad");

		texture->bind();

		if (shaderProgram->bind()) {
			auto color = _color;

			color.setAlphaF(actor()->opacity());

			shaderProgram->setUniformValue("imageTexture", 0);
			shaderProgram->setUniformValue("color", color);
			shaderProgram->setUniformValue("transform", modelViewProjectionMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		texture->release();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

void SelectionImageProp::setImage(std::shared_ptr<QImage> image)
{
	const auto texture = textureByName("Quad");

	texture->destroy();
	texture->setData(*image.get());
	texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);

	const auto rectangle = QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height())));

	shapeByName<QuadShape>("Quad")->setRectangle(rectangle);

	QMatrix4x4 modelMatrix;

	modelMatrix.translate(-0.5f * rectangle.width(), -0.5f * rectangle.height(), 0.0f);

	setModelMatrix(modelMatrix);

	emit changed(this);
}

QSize SelectionImageProp::imageSize() const
{
	if (!_initialized)
		return QSize();

	const auto quadRectangle = shapeByName<QuadShape>("Quad")->rectangle();

	return QSize(static_cast<int>(quadRectangle.width()), static_cast<int>(quadRectangle.height()));
}