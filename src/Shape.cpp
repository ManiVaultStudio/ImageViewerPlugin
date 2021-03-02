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

QString Shape::getFullName()
{
    return QString("%2::%3").arg(getProp()->name(), _name);
}

void Shape::initialize()
{
    //qDebug() << "Initialize" << fullName();

    Prop::renderer->bindOpenGLContext();

    _vao.create();
    _vbo.create();
}

void Shape::destroy()
{
    //qDebug() << "Destroy" << fullName();

    Prop::renderer->bindOpenGLContext();

    _vao.destroy();
    _vao.release();

    _vbo.destroy();
    _vbo.release();
}

void Shape::render()
{
    //qDebug() << "Render" << fullName();
}

QString Shape::getName() const
{
    return _name;
}

void Shape::setName(const QString& name)
{
    if (name == _name)
        return;

    const auto oldName = getFullName();

    _name = name;

    qDebug() << "Rename" << oldName << "to" << getFullName();
}

Prop* Shape::getProp()
{
    return _prop;
}