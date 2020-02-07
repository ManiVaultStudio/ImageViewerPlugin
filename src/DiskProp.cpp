#include "DiskProp.h"
#include "DiskShape.h"
#include "Actor.h"
#include "Renderer.h"

#include <QDebug>

const std::string vertexShaderSource =
#include "DiskVertex.glsl"
;

const std::string geometryShaderSource =
#include "DiskGeometry.glsl"
;

const std::string fragmentShaderSource =
#include "DiskFragment.glsl"
;

DiskProp::DiskProp(Actor* actor, const QString& name) :
	Prop(actor, name),
	_radius(1.0f),
	_color()
{
	addShape<DiskShape>("DiskShape");
	addShaderProgram("DiskShape");
}

void DiskProp::initialize()
{
	try
	{
		Prop::initialize();

		const auto shaderProgram = shaderProgramByName("DiskShape");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str()))
			throw std::exception("Unable to compile disk vertex shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Geometry, geometryShaderSource.c_str()))
			throw std::exception("Unable to compile disk geometry shader");

		if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource.c_str()))
			throw std::exception("Unable to compile disk fragment shader");

		if (!shaderProgram->link())
			throw std::exception("Unable to link disk shader program");

		const auto stride = 5 * sizeof(GLfloat);

		auto shape = shapeByName<DiskShape>("DiskShape");

		if (shaderProgram->bind()) {
			shape->vao().bind();
			shape->vbo().bind();

			shaderProgram->enableAttributeArray(0);
			shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);

			shape->vao().release();
			shape->vbo().release();
		}
		else {
			throw std::exception("Unable to bind disk shader program");
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

bool DiskProp::canRender() const
{
	if (!Prop::canRender())
		return false;

	if (_radius <= 0.0f)
		return false;

	return true;
}

void DiskProp::render()
{
	try {
		if (!canRender())
			return;

		Prop::render();

		const auto shape			= shapeByName<DiskShape>("DiskShape");
		const auto shaderProgram	= shaderProgramByName("DiskShape");

		if (shaderProgram->bind()) {
			shaderProgram->setUniformValue("transform", modelViewProjectionMatrix());
			shaderProgram->setUniformValue("screenToNormalizedScreenMatrix", renderer()->screenCoordinatesToNormalizedScreenCoordinatesMatrix());

			shape->render();

			shaderProgram->release();
		}
		else {
			throw std::exception("Unable to bind disk shader program");
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

void DiskProp::updateShapes()
{
	//qDebug() << "Update shapes" << fullName();

	const auto pA		= renderer()->worldPositionToScreenPoint(QVector3D(0.0f, 0.0f, 0.0f));
	const auto pB		= renderer()->worldPositionToScreenPoint(QVector3D(_radius, 0.0f, 0.0f));
	const auto radius	= (pB - pA).length();

	QVector<QVector3D> centers;

	centers << QVector3D(_center, 0.0f);
	
	shapeByName<DiskShape>("DiskShape")->setPoints(centers);
}

QVector2D DiskProp::center() const
{
	return _center;
}

void DiskProp::setCenter(const QVector2D& center)
{
	if (center == _center)
		return;

	_center = center;

	updateShapes();
}

float DiskProp::radius()
{
	return _radius;
}

void DiskProp::setRadius(const float& radius)
{
	if (radius == _radius)
		return;

	_radius = radius;

	updateShapes();
}

QColor DiskProp::color()
{
	return _color;
}

void DiskProp::color(const QColor& color)
{
	if (color == _color)
		return;

	_color = color;
}