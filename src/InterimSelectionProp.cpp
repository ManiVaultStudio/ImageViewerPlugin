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

const std::string bufferVertexShaderSource =
#include "InterimSelectionBufferVertex.glsl"
;

const std::string bufferFragmentShaderSource =
#include "InterimSelectionBufferFragment.glsl"
;

InterimSelectionProp::InterimSelectionProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_fbo()
{
}

void InterimSelectionProp::initialize()
{
	Prop::initialize();

	addShape<QuadShape>("QuadShape");

	addShaderProgram("Quad");
	
	const auto quadShaderProgram = shaderProgramByName("Quad");

	quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, quadVertexShaderSource.c_str());
	quadShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, quadFragmentShaderSource.c_str());

	if (!quadShaderProgram->link()) {
		throw std::exception("Unable to link interim selection prop quad shader program");
	}

	addShaderProgram("Buffer");

	const auto bufferShaderProgram = shaderProgramByName("Buffer");

	bufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, bufferVertexShaderSource.c_str());
	bufferShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, bufferFragmentShaderSource.c_str());

	if (!bufferShaderProgram->link()) {
		throw std::exception("Unable to link interim selection prop buffer shader program");
	}


	/*
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