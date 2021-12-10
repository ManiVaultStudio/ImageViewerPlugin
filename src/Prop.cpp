#include "Prop.h"
#include "LayersRenderer.h"
#include "Renderable.h"
#include "Shape.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QDebug>

Prop::Prop(Renderable& renderable, const QString& name) :
    _renderable(renderable),
    _initialized(false),
    _name(name),
    _visible(true),
    _modelMatrix(),
    _shaderPrograms(),
    _textures(),
    _shapes()
{
}

void Prop::initialize()
{
    //qDebug() << "Initialize" << fullName();

    _renderable.getRenderer().bindOpenGLContext();

    for (auto shape : _shapes) {
        shape->initialize();
    }
}

void Prop::destroy()
{
    //qDebug() << "Destroy" << fullName();

    _renderable.getRenderer().bindOpenGLContext();

    for (auto shape : _shapes) {
        shape->destroy();
    }
}

bool Prop::canRender() const
{
    return isInitialized() && isVisible();
}

void Prop::addShaderProgram(const QString& name)
{
    _shaderPrograms.insert(name, QSharedPointer<QOpenGLShaderProgram>::create());
}

QSharedPointer<QOpenGLShaderProgram> Prop::getShaderProgramByName(const QString& name)
{
    return _shaderPrograms.value(name);
}

void Prop::addTexture(const QString& name, const QOpenGLTexture::Target& target)
{
    _textures.insert(name, QSharedPointer<QOpenGLTexture>::create(target));
}

QSharedPointer<QOpenGLTexture>& Prop::getTextureByName(const QString& name)
{
    return _textures[name];
}

void Prop::render(const QMatrix4x4& modelViewProjectionMatrix)
{
    qDebug() << "Render not implemented in prop";
}

LayersRenderer& Prop::getRenderer()
{
    return _renderable.getRenderer();
}

bool Prop::isInitialized() const
{
    return _initialized;
}

QString Prop::getName() const
{
    return _name;
}

void Prop::setName(const QString& name)
{
    if (name == _name)
        return;

    const auto oldName = getFullName();

    _name = name;

    qDebug() << "Rename" << oldName << "to" << getFullName();
}

bool Prop::isVisible() const
{
    return _visible;
}

void Prop::setVisible(const bool& visible)
{
    if (visible == _visible)
        return;

    _visible = visible;

    qDebug() << (_visible ? "Show" : "Hide") << getFullName();
}

void Prop::show()
{
    setVisible(true);
}

void Prop::hide()
{
    setVisible(false);
}

QString Prop::getFullName()
{
    return _name;
}

QMatrix4x4 Prop::getModelMatrix() const
{
    return _modelMatrix;
}

void Prop::setModelMatrix(const QMatrix4x4& modelMatrix)
{
    if (modelMatrix == _modelMatrix)
        return;

    _modelMatrix = modelMatrix;
}

QRect Prop::getScreenBoundingRectangle() const
{
    return _renderable.getRenderer().getScreenRectangleFromWorldRectangle(getWorldBoundingRectangle());
}
