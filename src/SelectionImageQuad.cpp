#include "SelectionImageQuad.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

#include "Actor.h"

SelectionImageQuad::SelectionImageQuad(Actor* actor, const QString& name, const float& z /*= 0.f*/) :
	Quad(actor, name, z)
{
	_color = QColor(255, 0, 0, 255);
}

QSize SelectionImageQuad::size() const
{
	return QSize(static_cast<int>(_rectangle.width()), static_cast<int>(_rectangle.height()));
}

void SelectionImageQuad::setImage(std::shared_ptr<QImage> image)
{
	qDebug() << "Set selection image for" << _name;

	auto quadTexture = texture("Quad");

	texture("Quad")->destroy();
	texture("Quad")->setData(*image.get());
	texture("Quad")->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
	texture("Quad")->setWrapMode(QOpenGLTexture::ClampToEdge);

	setRectangle(QRectF(QPointF(), QSizeF(static_cast<float>(image->width()), static_cast<float>(image->height()))));

	emit changed(this);
}

void SelectionImageQuad::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	setShaderProgram("Quad", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionImageQuadVertexShaderSource.c_str());
	shaderProgram("Quad")->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionImageQuadFragmentShaderSource.c_str());
	shaderProgram("Quad")->link();
}

void SelectionImageQuad::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	setTexture("Quad", QSharedPointer<QOpenGLTexture>::create(QOpenGLTexture::Target2D));
}

void SelectionImageQuad::configureShaderProgram(const QString& name)
{
	Quad::configureShaderProgram(name);

	auto quadProgram = shaderProgram("Quad");

	_color.setAlphaF(_actor->opacity());

	if (name == "Quad") {
		quadProgram->setUniformValue("selectionTexture", 0);
		quadProgram->setUniformValue("color", _color);
	}
}