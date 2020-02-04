#include "Shape.h"
#include "Prop.h"
#include "Renderer.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QDebug>

Shape::Shape(Prop* prop, const QString& name) :
	_prop(prop),
	_name(name),
	_vao(),
	_vbo()
{
}

Shape::~Shape() = default;

QString Shape::fullName()
{
	return QString("%1::%2::%3").arg(prop()->actor()->name(), prop()->name(), _name);
}

void Shape::initialize()
{
	//qDebug() << "Initialize" << fullName();

	renderer()->bindOpenGLContext();

	_vao.create();
	_vbo.create();
}

void Shape::destroy()
{
	//qDebug() << "Destroy" << fullName();

	renderer()->bindOpenGLContext();

	_vao.destroy();
	_vao.release();

	_vbo.destroy();
	_vbo.release();
}

void Shape::render()
{
	//qDebug() << "Render" << fullName();
}

QString Shape::name() const
{
	return _name;
}

void Shape::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = fullName();

	_name = name;

	qDebug() << "Rename" << oldName << "to" << fullName();
}

Prop* Shape::prop()
{
	return _prop;
}

Renderer* Shape::renderer()
{
	return _prop->renderer();
}