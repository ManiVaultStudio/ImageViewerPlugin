#include "Shape.h"
#include "Prop.h"
#include "LayersRenderer.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QDebug>

Shape::Shape(Prop& prop, const QString& name) :
    _prop(prop),
    _name(name),
    _vao(),
    _vbo()
{
}

QString Shape::getFullName()
{
    return QString("%2::%3").arg(_prop.getName(), _name);
}

void Shape::initialize()
{
    auto& renderer = _prop.getRenderer();

    renderer.bindOpenGLContext();
    {
        _vao.create();
        _vbo.create();
    }
    renderer.releaseOpenGLContext();
}

void Shape::destroy()
{
    auto& renderer = _prop.getRenderer();

    renderer.bindOpenGLContext();
    {
        _vao.destroy();
        _vao.release();

        _vbo.destroy();
        _vbo.release();
    }
    renderer.releaseOpenGLContext();
}

void Shape::render()
{
    qDebug() << "Shape render not implemented";
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

Prop& Shape::getProp()
{
    return _prop;
}
