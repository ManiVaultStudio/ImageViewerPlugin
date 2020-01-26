#include "SelectionBounds.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QDebug>

#include "Shaders.h"

SelectionBounds::SelectionBounds(Renderer* renderer, const QString& name, const float& z /*= 0.f*/, const QColor& color /*= QColor(255, 153, 0, 150)*/) :
	Polyline2D(renderer, name, z),
	_bounds(),
	_color(color)
{
}

void SelectionBounds::setBounds(const QRectF& bounds)
{
	if (bounds == _bounds)
		return;

	qDebug() << "Set selection bounds shape bounds";

	_bounds = bounds;
	
	const auto p0 = _bounds.topLeft();
	const auto p1 = _bounds.bottomRight() - QPointF(1.f, 1.f);

	QVector<QVector2D> points;

	points.append(QVector2D(p0.x(), p0.y()));
	points.append(QVector2D(p1.x(), p0.y()));
	points.append(QVector2D(p1.x(), p1.y()));
	points.append(QVector2D(p0.x(), p1.y()));

	setPoints(points);

	emit boundsChanged(_bounds);
}

void SelectionBounds::setColor(const QColor& color)
{
	if (color == _color)
		return;

	qDebug() << "Set selection bounds shape color";

	_color = color;

	emit colorChanged(_color);
}

void SelectionBounds::addShaderPrograms()
{
	qDebug() << "Add OpenGL shader programs to" << _name << "shape";

	addShaderProgram("Polyline", QSharedPointer<QOpenGLShaderProgram>::create());

	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionBoundsVertexShaderSource.c_str());
	shaderProgram("Polyline")->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	shaderProgram("Polyline")->link();
}

void SelectionBounds::addVAOs()
{
	qDebug() << "Add OpenGL VAO's to" << _name << "shape";

	addVAO("Polyline", QSharedPointer<QOpenGLVertexArrayObject>::create());

	vao("Polyline")->create();
}

void SelectionBounds::addVBOs()
{
	qDebug() << "Add OpenGL VBO's to" << _name << "shape";

	addVBO("Polyline", QSharedPointer<QOpenGLBuffer>::create());

	vbo("Polyline")->create();
}

void SelectionBounds::addTextures()
{
	qDebug() << "Add OpenGL textures to" << _name << "shape";

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _color);

	addTexture("Polyline", QSharedPointer<QOpenGLTexture>::create(textureImage));

	texture("Polyline")->setWrapMode(QOpenGLTexture::Repeat);
	texture("Polyline")->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
}

void SelectionBounds::configureShaderProgram(const QString& name)
{
	Polyline2D::configureShaderProgram(name);

	auto polylineProgram = shaderProgram("Polyline");

	if (name == "Polyline") {
		QMatrix4x4 translate;

		translate.translate(0.f, 0.f, _z);

		polylineProgram->setUniformValue("transform", modelViewProjectionMatrix() * translate);
	}
}