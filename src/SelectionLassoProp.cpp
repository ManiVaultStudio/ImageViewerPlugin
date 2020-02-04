#include "SelectionLassoProp.h"
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

SelectionLassoProp::SelectionLassoProp(Actor* actor, const QString& name) :
	Prop(actor, name)
{
}

void SelectionLassoProp::initialize()
{
	Prop::initialize();

	/*
	const auto quadShapeShaderProgram = _shaderPrograms["QuadShape"];

	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	quadShapeShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!quadShapeShaderProgram->link()) {
		throw std::exception("Unable to link color image quad shader program");
	}

	const auto stride = 5 * sizeof(GLfloat);

	auto quadShape = shape<QuadShape>("QuadShape");

	if (quadShapeShaderProgram->bind()) {
		quadShape->vao().bind();
		quadShape->vbo().bind();

		quadShapeShaderProgram->enableAttributeArray(QuadShape::_vertexAttribute);
		quadShapeShaderProgram->enableAttributeArray(QuadShape::_textureAttribute);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_vertexAttribute, GL_FLOAT, 0, 3, stride);
		quadShapeShaderProgram->setAttributeBuffer(QuadShape::_textureAttribute, GL_FLOAT, 3 * sizeof(GLfloat), 2, stride);
		quadShapeShaderProgram->release();

		quadShape->vao().release();
		quadShape->vbo().release();
	}
	else {
		throw std::exception("Unable to bind color image quad shader program");
	}

	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->setWrapMode(QOpenGLTexture::Repeat);
	quadShapeTexture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
	*/
}

void SelectionLassoProp::render()
{
	/*
	if (!canRender())
		return;

	Prop::render();

	const auto quadShape = _shapes["QuadShape"];
	const auto quadShapeShaderProgram = _shaderPrograms["QuadShape"];
	const auto quadShapeTexture = _textures["QuadShape"];

	quadShapeTexture->bind();

	if (quadShapeShaderProgram->bind()) {
		auto color = _color;

		color.setAlphaF(actor()->opacity());

		quadShapeShaderProgram->setUniformValue("imageTexture", 0);
		quadShapeShaderProgram->setUniformValue("color", color);
		quadShapeShaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);

		quadShape->render();

		quadShapeShaderProgram->release();
	}

	quadShapeTexture->release();
	*/
}