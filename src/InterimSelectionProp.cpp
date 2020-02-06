#include "InterimSelectionProp.h"
#include "QuadShape.h"
#include "Actor.h"

#include <QDebug>

const std::string quadVertexShaderSource =
#include "InterimSelectionQuadVertex.glsl"
;

const std::string quadFragmentShaderSource =
#include "InterimSelectionQuadFragment.glsl"
;

const std::string offscreenBufferVertexShaderSource =
#include "InterimSelectionOffscreenBufferVertex.glsl"
;

const std::string offscreenBufferFragmentShaderSource =
#include "InterimSelectionOffscreenBufferFragment.glsl"
;

InterimSelectionProp::InterimSelectionProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_fbo()
{
	addShape<QuadShape>("Quad");
	addShaderProgram("Quad");
	addShaderProgram("OffscreenBuffer");
}

void InterimSelectionProp::initialize()
{
	try
	{
		Prop::initialize();

		auto shape = shapeByName<QuadShape>("Quad");

		const auto stride = 5 * sizeof(GLfloat);

		const auto quadShaderProgram = shaderProgramByName("Quad");

		if (!quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, quadVertexShaderSource.c_str()))
			throw std::exception("Unable to compile quad vertex shader");

		if (!quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, quadFragmentShaderSource.c_str()))
			throw std::exception("Unable to compile quad fragment shader");

		if (!quadShaderProgram->link())
			throw std::exception("Unable to link quad shader program");

		if (quadShaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			quadShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
			quadShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
			quadShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
			quadShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

			shape->vao().release();
			shape->vbo().release();

			quadShaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind quad shader program");
		}

		const auto offscreenBufferShaderProgram = shaderProgramByName("OffscreenBuffer");

		if (!offscreenBufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, offscreenBufferVertexShaderSource.c_str()))
			throw std::exception("Unable to compile off screen buffer vertex shader");

		if (!offscreenBufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, offscreenBufferFragmentShaderSource.c_str()))
			throw std::exception("Unable to compile off screen buffer fragment shader");

		if (!offscreenBufferShaderProgram->link())
			throw std::exception("Unable to link off screen buffer shader program");

		if (offscreenBufferShaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			offscreenBufferShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
			offscreenBufferShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
			offscreenBufferShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
			offscreenBufferShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);

			shape->vao().release();
			shape->vbo().release();

			offscreenBufferShaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind off screen buffer shader program");
		}

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

void InterimSelectionProp::render()
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

void InterimSelectionProp::setImageSize(const QSize& imageSize)
{
	if (!_fbo.isNull() && imageSize == _fbo->size())
		return;

	qDebug() << "Set image size to" << imageSize;

	_fbo.reset(new QOpenGLFramebufferObject(imageSize.width(), imageSize.height()));
}