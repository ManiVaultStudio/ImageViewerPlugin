#include "SelectionImageProp.h"
#include "QuadShape.h"

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
}

void SelectionImageProp::setImage(std::shared_ptr<QImage> image)
{
	qDebug() << "Set selection image for" << _name;

	/*
	auto quadTexture = texture("Quad");

	texture("Quad")->destroy();
	texture("Quad")->setData(*image.get());
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture("Quad")->setWrapMode(QOpenGLTexture::ClampToEdge);

	setRectangle(QRectF(QPointF(), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	emit changed(this);
	*/

	emit sizeChanged(image->size());
}

QSize SelectionImageProp::imageSize() const
{
	if (!_initialized)
		return QSize();

	const auto quadRectangle = dynamic_cast<QuadShape*>(_shapes["Quad"].get())->rectangle();

	return QSize(static_cast<int>(quadRectangle.width()), static_cast<int>(quadRectangle.height()));
}

void SelectionImageProp::initialize()
{
	Prop::initialize();

	_shapes["Quad"] = QSharedPointer<QuadShape>::create(this, "Quad");

	_shaderPrograms["Quad"] = QSharedPointer<QOpenGLShaderProgram>::create();

	_shaderPrograms["Quad"]->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_shaderPrograms["Quad"]->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!_shaderPrograms["Quad"]->link()) {
		throw std::exception("Unable to link selection image quad shader program");
	}

	_textures["Quad"] = QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D);

	_textures["Quad"]->setWrapMode(QOpenGLTexture::Repeat);
	_textures["Quad"]->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
}

void SelectionImageProp::render()
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

/*
void SelectionImage::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	setShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, SelectionImageVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, SelectionImageFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void SelectionImage::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	setTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));
}

void SelectionImage::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	_color.setAlphaF(_actor->opacity());

	if (name == "Quad") {
		quadProgram->setUniformValue("selectionTexture", 0);
		quadProgram->setUniformValue("color", _color);
	}
}
*/