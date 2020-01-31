#include "ColorImageQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

ColorImageQuad::ColorImageQuad(Actor* actor, const QString& name, const float& z /*= 0.f*/) :
	Quad(actor, name, z),
	_minPixelValue(),
	_maxPixelValue()
{
}

QSize ColorImageQuad::imageSize() const
{
	return QSize(static_cast<int>(_rectangle.width()), static_cast<int>(_rectangle.height()));
}

void ColorImageQuad::setMinPixelValue(const float& minPixelValue)
{
	if (minPixelValue == _minPixelValue)
		return;

	_minPixelValue = minPixelValue;
}

void ColorImageQuad::setMaxPixelValue(const float& maxPixelValue)
{
	if (maxPixelValue == _maxPixelValue)
		return;

	_maxPixelValue = maxPixelValue;
}

void ColorImageQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	setShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, imageQuadVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, imageQuadFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void ColorImageQuad::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	setTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));

	texture("Quad")->setWrapMode(QOpenGLTexture::Repeat);
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void ColorImageQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("imageTexture", 0);
		quadProgram->setUniformValue("minPixelValue", _minPixelValue);
		quadProgram->setUniformValue("maxPixelValue", _maxPixelValue);
	}
}