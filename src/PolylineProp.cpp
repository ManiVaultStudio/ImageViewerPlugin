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
	_closed(true),
	_lineWidth(0.01f),
	_lineColor(255, 160, 0, 100)
{
	addShape<PolylineShape>("PolylineShape");
	addShaderProgram("PolylineShape");
	addTexture("Polyline", QOpenGLTexture::Target2D);
}

bool PolylineProp::canRender() const
{
	if (!Prop::canRender())
		return false;

	if (!shapeByName<PolylineShape>("PolylineShape")->canRender())
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

bool PolylineProp::closed()
{
	return _closed;
}

void PolylineProp::setClosed(const bool& closed)
{
	if (closed == _closed)
		return;

	_closed = closed;

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

QColor PolylineProp::lineColor()
{
	return _lineColor;
}

void PolylineProp::setLineColor(const QColor& lineColor)
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

	texture->create();

	updateTextures();

	_initialized = true;
}

void PolylineProp::render()
{
	if (!canRender())
		return;

	Prop::render();

	//qDebug() << "Render" << fullName();

	const auto shape			= shapeByName<PolylineShape>("PolylineShape");
	const auto shaderProgram	= shaderProgramByName("PolylineShape");
	const auto texture			= textureByName("Polyline");

	texture->bind();
	{
		if (shaderProgram->bind()) {
			shaderProgram->setUniformValue("screenToNormalizedScreenMatrix", renderer()->screenCoordinatesToNormalizedScreenCoordinatesMatrix());
			shaderProgram->setUniformValue("lineTexture", 0);
			shaderProgram->setUniformValue("lineWidth", _lineWidth);

			shape->render();

			shaderProgram->release();
		}
	}
	texture->release();
}

void PolylineProp::updateShapes()
{
	if (_points.size() < 2)
		return;

	//qDebug() << "Update shapes" << fullName();

	QVector<PolylineShape::Point> polylinePoints;

	for (auto point : _points) {
		polylinePoints.push_back(PolylineShape::Point(point, QVector2D(0.f, 0.f), _lineWidth));
	}
	
	if (_closed) {
		polylinePoints.insert(0, polylinePoints.back());
		polylinePoints.append(polylinePoints[1]);
		polylinePoints.append(polylinePoints[2]);
	}
	else {
		polylinePoints.insert(0, polylinePoints.first());
		polylinePoints.append(polylinePoints.back());
	}

	/*
	if (!polylinePoints.size() > 3) {
		polylinePoints.insert(0, polylinePoints[polylinePoints.size() - 2]);
		polylinePoints.append(polylinePoints[1]);
	}
	*/

	shapeByName<PolylineShape>("PolylineShape")->setPoints(polylinePoints);
}

void PolylineProp::updateTextures()
{
	//qDebug() << "Update textures" << fullName();

	auto textureImage = QImage(1, 1, QImage::Format::Format_RGBA8888);

	textureImage.setPixelColor(QPoint(0, 0), _lineColor);

	auto texture = textureByName("Polyline");

	texture->destroy();
	texture->create();
	texture->setWrapMode(QOpenGLTexture::Repeat);
	texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
	texture->setData(textureImage);
}