#include "PointsProp.h"
#include "Actor.h"
#include "Renderer.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "PointsVertex.glsl"
;

const std::string geometryShaderSource =
#include "PointsGeometry.glsl"
;

const std::string fragmentShaderSource =
#include "PointsFragment.glsl"
;

PointsProp::Point::Point():
	_position(),
	_radius(),
	_color()
{
}

PointsProp::Point::Point(const QVector3D& position, const float& radius, const QColor& color) :
	_position(position),
	_radius(radius),
	_color(QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF()))
{
}

std::uint32_t PointsProp::Point::_positionAttribute = 0;
std::uint32_t PointsProp::Point::_radiusAttribute	= 1;
std::uint32_t PointsProp::Point::_colorAttribute	= 2;

PointsProp::PointsProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_points(),
	_vao(),
	_vbo(),
	_noSegments(32)
{
	addShaderProgram("PointsShape");
}

void PointsProp::initialize()
{
	try
	{
		Prop::initialize();

		const auto shaderProgram = shaderProgramByName("PointsShape");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
			throw std::exception("Unable to compile Points vertex shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str()))
			throw std::exception("Unable to compile Points geometry shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
			throw std::exception("Unable to compile Points fragment shader");

		if (!shaderProgram->link())
			throw std::exception("Unable to link Points shader program");

		const auto stride = sizeof(Point);

		_vao.create();
		_vbo.create();

		if (shaderProgram->bind()) {
			_vao.bind();
			_vbo.bind();

			shaderProgram->enableAttributeArray(Point::_positionAttribute);
			shaderProgram->enableAttributeArray(Point::_radiusAttribute);
			shaderProgram->enableAttributeArray(Point::_colorAttribute);
			shaderProgram->setAttributeBuffer(Point::_positionAttribute, GL_FLOAT, 0, 3, stride);
			shaderProgram->setAttributeBuffer(Point::_radiusAttribute, GL_FLOAT, sizeof(QVector3D), 1, stride);
			shaderProgram->setAttributeBuffer(Point::_colorAttribute, GL_FLOAT, sizeof(QVector3D) + sizeof(float), 4, stride);

			_vao.release();
			_vbo.release();
		}
		else {
			throw std::exception("Unable to bind Points shader program");
		}

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

bool PointsProp::canRender() const
{
	if (!Prop::canRender())
		return false;

	if (_points.size() <= 0)
		return false;

	return true;
}

void PointsProp::render()
{
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shaderProgram = shaderProgramByName("PointsShape");

		if (shaderProgram->bind()) {
			shaderProgram->setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix() * renderer()->normalizedScreenToScreenMatrix());
			shaderProgram->setUniformValue("screenCoordinates", true);
			shaderProgram->setUniformValue("noSegments", _noSegments);
			shaderProgram->setUniformValue("screenToNormalizedScreen", renderer()->screenToNormalizedScreenMatrix());

			_vao.bind();
			{
				glDrawArrays(GL_POINTS, 0, _points.size());
			}
			_vao.release();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind Points shader program");
		}
	}
	catch (std::exception& e)
	{
		qDebug() << _name << "render failed:" << e.what();
	}
	catch (...) {
		qDebug() << _name << "render failed due to unhandled exception";
	}
}

void PointsProp::setPoints(const QVector<Point>& points)
{
	_points = points;

	_vao.bind();
	{
		_vbo.bind();
		{
			_vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
			_vbo.allocate(_points.constData(), _points.count() * sizeof(Point));
		}
		_vbo.release();
	}
	_vao.release();
}

std::uint32_t PointsProp::noSegments() const
{
	return _noSegments;
}

void PointsProp::setNoSegments(const std::uint32_t& noSegments)
{
	_noSegments = noSegments;
}