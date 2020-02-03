#include "Prop.h"
#include "Renderer.h"
#include "Shape.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFramebufferObject>
#include <QDebug>

Prop::Prop(Actor* actor, const QString& name) :
	_actor(actor),
	_initialized(false),
	_name(name),
	_visible(true),
	_matrix(),
	_shaderPrograms(),
	_textures(),
	_shapes()
{
}

Prop::~Prop() = default;

void Prop::destroy()
{
	qDebug() << "Destroy prop" << _name;

	bindOpenGLContext();
}

void Prop::initialize()
{
	qDebug() << "Initialize the" << _name << "Prop";

	bindOpenGLContext();
}

bool Prop::isInitialized() const
{
	return _initialized;
}

QString Prop::name() const
{
	return _name;
}

void Prop::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = _name;

	_name = name;

	qDebug() << "Rename" << oldName << "to" << _name;

	emit nameChanged(_name);

	emit changed(this);
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

	qDebug() << (_visible ? "Show" : "Hide") << _name;

	emit visibilityChanged(_visible);

	emit changed(this);
}

void Prop::show()
{
	setVisible(true);
}

void Prop::hide()
{
	setVisible(false);
}

bool Prop::canRender() const
{
	return isInitialized() && isVisible();
}

Actor* Prop::actor()
{
	return _actor;
}

void Prop::render()
{
	qDebug() << "Render" << _name << "prop";
}

void Prop::bindOpenGLContext()
{
	renderer()->bindOpenGLContext();
}

void Prop::releaseOpenGLContext()
{
	renderer()->releaseOpenGLContext();
}

Renderer* Prop::renderer()
{
	return _actor->renderer();
}