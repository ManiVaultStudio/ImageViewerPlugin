#include "Prop.h"
#include "Renderer.h"
#include "Actor.h"
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
	_modelMatrix(),
	_shaderPrograms(),
	_textures(),
	_shapes()
{
}

Prop::~Prop() = default;

void Prop::initialize()
{
	//qDebug() << "Initialize" << fullName();

	renderer()->bindOpenGLContext();

	for (auto shape : _shapes) {
		shape->initialize();
	}
}

void Prop::destroy()
{
	//qDebug() << "Destroy" << fullName();

	renderer()->bindOpenGLContext();

	for (auto shape : _shapes) {
		shape->destroy();
	}
}

bool Prop::canRender() const
{
	if (!_actor->canRender())
		return false;

	return isInitialized() && isVisible();
}

void Prop::render()
{
	//qDebug() << "Render" << fullName();
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

	const auto oldName = fullName();

	_name = name;

	qDebug() << "Rename" << oldName << "to" << fullName();
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

	qDebug() << (_visible ? "Show" : "Hide") << fullName();
}

void Prop::show()
{
	setVisible(true);
}

void Prop::hide()
{
	setVisible(false);
}

QString Prop::fullName()
{
	return QString("%2::%3").arg(actor()->name(), _name);
}

QMatrix4x4 Prop::modelMatrix() const
{
	return _modelMatrix;
}

void Prop::setModelMatrix(const QMatrix4x4& modelMatrix)
{
	if (modelMatrix == _modelMatrix)
		return;

	_modelMatrix = modelMatrix;
}

QMatrix4x4 Prop::modelViewMatrix() const
{
	return _actor->modelViewMatrix() * _modelMatrix;
}

QMatrix4x4 Prop::modelViewProjectionMatrix() const
{
	return _actor->modelViewProjectionMatrix() * _modelMatrix;
}

Actor* Prop::actor()
{
	return _actor;
}

Renderer* Prop::renderer()
{
	return _actor->renderer();
}