#include "SelectionBufferProp.h"
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

SelectionBufferProp::SelectionBufferProp(Actor* actor, const QString& name) :
	Prop(actor, name)
{
	addShape<QuadShape>("QuadShape");
	addShaderProgram("QuadShape");
	addTexture("QuadShape", QOpenGLTexture::Target2D);
}

void SelectionBufferProp::setImage(std::shared_ptr<QImage> image)
{
	const auto texture = textureByName("QuadShape");

	texture->destroy();
	texture->setData(*image.get());
	texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture->setWrapMode(QOpenGLTexture::ClampToEdge);

	shapeByName<QuadShape>("QuadShape")->setRectangle(QRectF(QPointF(0.f, 0.f), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	emit changed(this);
}

QSize SelectionBufferProp::imageSize() const
{
	if (!_initialized)
		return QSize();

	const auto quadRectangle = shapeByName<QuadShape>("QuadShape")->rectangle();

	return QSize(static_cast<int>(quadRectangle.width()), static_cast<int>(quadRectangle.height()));
}

void SelectionBufferProp::initialize()
{
	Prop::initialize();

	/*
	const auto shaderProgram = shaderProgramByName("QuadShape");

	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!shaderProgram->link()) {
		throw std::exception("Unable to link color image quad shader program");
	}

	const auto stride = 5 * sizeof(GLfloat);

	auto shape = shapeByName<QuadShape>("QuadShape");

	if (shaderProgram->bind()) {
		shape->vao().bind();
		shape->vbo().bind();

		shaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
		shaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
		shaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		shaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		shaderProgram->release();

		shape->vao().release();
		shape->vbo().release();
	}
	else {
		throw std::exception("Unable to bind color image quad shader program");
	}

	const auto texture = textureByName("QuadShape");

	texture->setWrapMode(QOpenGLTexture::Repeat);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
	*/
}

void SelectionBufferProp::render()
{
	/*
	if (!canRender())
		return;

	Prop::render();

	const auto shape			= shapeByName<QuadShape>("QuadShape");
	const auto shaderProgram	= shaderProgramByName("QuadShape");
	const auto texture			= textureByName("QuadShape");

	texture->bind();

	if (shaderProgram->bind()) {
		auto color = _color;

		color.setAlphaF(actor()->opacity());

		shaderProgram->setUniformValue("imageTexture", 0);
		shaderProgram->setUniformValue("color", color);
		shaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);

		shape->render();

		shaderProgram->release();
	}

	texture->release();
	*/
}