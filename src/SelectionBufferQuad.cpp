#include "SelectionBufferQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

SelectionBufferQuad::SelectionBufferQuad(const QString& name /*= "SelectionBufferQuad"*/, const float& z /*= 0.f*/) :
	Quad(name, z),
	_color(255, 0, 0, 200)
{
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

void SelectionBufferQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("selectionTexture", 0);
		quadProgram->setUniformValue("color", _color);
	}
}