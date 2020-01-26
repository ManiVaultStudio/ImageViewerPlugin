#include "SelectionQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

SelectionQuad::SelectionQuad(const QString& name /*= "SelectionQuad"*/, const float& z /*= 0.f*/) :
	Quad(name, z),
	_color(255, 0, 0, 200)
{
}

void SelectionQuad::setImage(std::shared_ptr<QImage> image)
{
	qDebug() << "Set selection image for" << _name;

	auto quadTexture = texture("Quad");

	texture("Quad")->destroy();
	texture("Quad")->setData(*image.get());
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture("Quad")->setWrapMode(QOpenGLTexture::ClampToEdge);

	setRectangle(QRectF(0, 0, image->width(), image->height()));

	emit changed(this);
}

float SelectionQuad::opacity() const
{
	return _color.alphaF();
}

void SelectionQuad::setOpacity(const float& opacity)
{
	if (opacity == _color.alphaF())
		return;

	_color.setAlphaF(opacity);

	qDebug() << "Set opacity" << _color.alphaF();

	emit opacityChanged(_color.alphaF());

	emit changed(this);
}

void SelectionQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void SelectionQuad::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	addTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));
}

void SelectionQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("selectionTexture", 0);
		quadProgram->setUniformValue("color", _color);
	}
}