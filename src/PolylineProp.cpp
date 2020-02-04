#include "PolylineProp.h"
#include "Actor.h"
#include "Renderer.h"
#include "PolylineShape.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "PolylineShapeVertex.glsl"
;

const std::string geometryShaderSource =
#include "PolylineShapeGeometry.glsl"
;

const std::string fragmentShaderSource =
#include "PolylineShapeFragment.glsl"
;

PolylineProp::PolylineProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_lineWidth(0.1f)
{
	addShape<PolylineShape>("PolylineShape");
	addShaderProgram("PolylineShape");
	addTexture("PolylineShape", QOpenGLTexture::Target2D);
}

bool PolylineProp::canRender() const
{
	if (!Prop::canRender())
		return false;

	if (_lineWidth <= 0.f)
		return false;

	return true;
}

QVector<QVector3D> PolylineProp::points() const
{
	return _points;
}

void PolylineProp::setPoints(const QVector<QVector3D>& points)
{
	if (points == _points)
		return;

	_points = points;

	updateShapes();
}

float PolylineProp::lineWidth()
{
	return _lineWidth;
}

void PolylineProp::setLineWidth(const float& lineWidth)
{
	if (lineWidth == _lineWidth)
		return;

	_lineWidth = lineWidth;

	updateShapes();
}

float PolylineProp::lineColor()
{
	return _lineColor;
}

void PolylineProp::setLineColor(const float& lineColor)
{
	if (lineColor == _lineColor)
		return;

	_lineColor = lineColor;

	updateTextures();
}

void PolylineProp::initialize()
{
	Prop::initialize();

	qDebug() << "Initialize" << fullName();

	const auto shaderProgram = shaderProgramByName("PolylineShape");

	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str());
	shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str());

	if (!shaderProgram->link()) {
		throw std::exception("Unable to link polyline shader program");
	}

	const auto stride = 5 * sizeof(GLfloat);

	auto shape = shapeByName<PolylineShape>("PolylineShape");

	if (shaderProgram->bind()) {
		shape->vao().bind();
		shape->vbo().bind();

		shaderProgram->enableAttributeArray(PolylineShape::Point::_positionAttribute);
		shaderProgram->enableAttributeArray(PolylineShape::Point::_textureCoordinateAttribute);
		shaderProgram->enableAttributeArray(PolylineShape::Point::_lineWidthAttribute);

		const auto stride = 6 * sizeof(GLfloat);

		shaderProgram->setAttributeBuffer(PolylineShape::Point::_positionAttribute, GL_FLOAT, 0, 3, stride);
		shaderProgram->setAttributeBuffer(PolylineShape::Point::_textureCoordinateAttribute, GL_FLOAT, 3, 3, stride);
		shaderProgram->setAttributeBuffer(PolylineShape::Point::_lineWidthAttribute, GL_FLOAT, 5, 3, stride);

		shape->vao().release();
		shape->vbo().release();
	}
	else {
		throw std::exception("Unable to bind color image quad shader program");
	}

	const auto texture = textureByName("Polyline");

	texture->setWrapMode(QOpenGLTexture::Repeat);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

	_initialized = true;
}

void PolylineProp::render()
{
	if (!canRender())
		return;

	Prop::render();

	qDebug() << "Render" << fullName();

	const auto shape			= shapeByName<PolylineShape>("PolylineShape");
	const auto shaderProgram	= shaderProgramByName("PolylineShape");
	const auto texture			= textureByName("PolylineShape");

	texture->bind();

	if (shaderProgram->bind()) {
		shaderProgram->setUniformValue("transform", actor()->modelViewProjectionMatrix() * _matrix);
		shaderProgram->setUniformValue("imageTexture", 0);
		shaderProgram->setUniformValue("lineWidth", _lineWidth);

		shape->render();

		shaderProgram->release();
	}

	texture->release();
}

void PolylineProp::updateShapes()
{
	qDebug() << "Update shapes" << fullName();

	QVector<PolylineShape::Point> polylinePoints;

	for (auto point : _points) {
		polylinePoints.push_back(PolylineShape::Point(point, QVector2D(0.f, 0.f), _lineWidth));
	}

	polylinePoints.insert(0, polylinePoints.first());
	polylinePoints.append(polylinePoints.back());

	shapeByName<PolylineShape>("PolylineShape")->setPoints(polylinePoints);
}

void PolylineProp::updateTextures()
{
	qDebug() << "Update textures" << fullName();

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), QColor(255, 0, 0));

	auto texture = textureByName("Polyline");

	texture->destroy();
	texture->create();
	texture->setWrapMode(QOpenGLTexture::Repeat);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture->setData(textureImage);
}