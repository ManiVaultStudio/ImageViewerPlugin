#include "PolylineProp.h"
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

PolylineProp::PolylineProp(Node* node, const QString& name) :
	Prop(node, name),
	_closed(true),
	_lineWidth(0.01f),
	_lineColor(255, 255, 255, 255)
{
	addShape<PolylineShape>("PolylineShape");
	addShaderProgram("PolylineShape");
	addTexture("Polyline", QOpenGLTexture::Target2D);
}

void PolylineProp::initialize()
{
	try
	{
		Prop::initialize();

		const auto shaderProgram = shaderProgramByName("PolylineShape");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
			throw std::exception("Unable to compile polyline vertex shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str()))
			throw std::exception("Unable to compile polyline geometry shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
			throw std::exception("Unable to compile polyline fragment shader");

		if (!shaderProgram->link())
			throw std::exception("Unable to link polyline shader program");

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
			throw std::exception("Unable to bind polyline shader program");
		}

		const auto texture = textureByName("Polyline");

		texture->create();

		updateTextures();

		_initialized = true;
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "initialization failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "initialization failed due to unhandled exception";
	}
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

void PolylineProp::render(const QMatrix4x4& nodeMVP, const float& opacity)
{
	/*
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shape			= shapeByName<PolylineShape>("PolylineShape");
		const auto shaderProgram	= shaderProgramByName("PolylineShape");
		const auto texture			= textureByName("Polyline");

		texture->bind();
		{
			if (shaderProgram->bind()) {
				shaderProgram->setUniformValue("screenToNormalizedScreenMatrix", renderer()->screenToNormalizedScreenMatrix());
				shaderProgram->setUniformValue("lineTexture", 0);
				shaderProgram->setUniformValue("lineWidth", _lineWidth);

				shape->render();

				shaderProgram->release();
			}
			else {
				throw std::exception("Unable to bind polyline shader program");
			}
		}

		texture->release();
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
	*/
}

void PolylineProp::updateShapes()
{
	//qDebug() << "Update shapes" << fullName();

	QVector<PolylineShape::Point> polylinePoints;

	if (_points.size() >= 2) {
		for (auto point : _points) {
			polylinePoints.append(PolylineShape::Point(point, QVector2D(0.f, 0.f), _lineWidth));
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
	}

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