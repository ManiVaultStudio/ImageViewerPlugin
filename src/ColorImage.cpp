#include "ColorImage.h"
#include "Quad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

const std::string vertexShaderSource =
#include "ImageQuadVertex.glsl"
;

const std::string fragmentShaderSource =
#include "ImageQuadFragment.glsl"
;

ColorImage::ColorImage(Actor* actor, const QString& name) :
	Prop(actor, name),
	_minPixelValue(),
	_maxPixelValue()
{
}

QSize ColorImage::imageSize() const
{
	if (!_initialized)
		return QSize();
	
	const auto quadRectangle = dynamic_cast<Quad*>(_shapes["Quad"].get())->rectangle();

	return QSize(static_cast<int>(quadRectangle.width()), static_cast<int>(quadRectangle.height()));
}

void ColorImage::setMinPixelValue(const float& minPixelValue)
{
	if (minPixelValue == _minPixelValue)
		return;

	_minPixelValue = minPixelValue;
}

void ColorImage::setMaxPixelValue(const float& maxPixelValue)
{
	if (maxPixelValue == _maxPixelValue)
		return;

	_maxPixelValue = maxPixelValue;
}

void ColorImage::initialize()
{
	Prop::initialize();

	_shapes["Quad"] = QSharedPointer<Quad>::create(this, "Quad");

	_shaderPrograms["Quad"] = QSharedPointer<QOpenGLShaderProgram>::create();

	_shaderPrograms["Quad"]->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_shaderPrograms["Quad"]->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!_shaderPrograms["Quad"]->link()) {
		throw std::exception("Unable to link color image quad shader program");
	}

	_textures["Quad"] = QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D);

	_textures["Quad"]->setWrapMode(QOpenGLTexture::Repeat);
	_textures["Quad"]->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
}

void ColorImage::render()
{
	/*
	auto quadProgram = shaderProgram("Quad");

	if (name == "Quad") {
		quadProgram->setUniformValue("imageTexture", 0);
		quadProgram->setUniformValue("minPixelValue", _minPixelValue);
		quadProgram->setUniformValue("maxPixelValue", _maxPixelValue);
	}
	*/
}