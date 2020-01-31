#include "Actor.h"

#include <QDebug>

#include "Renderer.h"

Actor::Actor(Renderer* renderer, const QString& name) :
	_renderer(renderer),
	_name(name),
	_active(false),
	_registeredEvents(static_cast<int>(ActorEvent::None)),
	_enabled(true),
	_modelMatrix(),
	_shapes()
{
}

Actor::~Actor() = default;

void Actor::destroy()
{
	qDebug() << "Destroy" << _name;

	bindOpenGLContext();
	
	for (auto name : _shapes.keys()) {
		_shapes[name]->destroy();
	}
}

void Actor::initialize()
{
	qDebug() << "Initialize" << _name;

	bindOpenGLContext();

	for (auto name : _shapes.keys()) {
		_shapes[name]->initialize();
	}
}

void Actor::render()
{
	qDebug() << "Render" << _name;

	for (auto name : _shapes.keys()) {
		_shapes[name]->render();
	}
}

QString Actor::name() const
{
	return _name;
}

void Actor::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = _name;

	_name = name;

	qDebug() << "Rename" << oldName << "to" << _name;

	emit nameChanged(_name);

	emit changed(this);
}

bool Actor::isEnabled() const
{
	return _enabled;
}

void Actor::setEnabled(const bool& enabled)
{
	if (enabled == _enabled)
		return;

	_enabled = enabled;

	qDebug() << "Set " << _name << "shape" << (_enabled ? "enabled" : "disabled");

	emit enabledChanged(enabled);

	emit changed(this);
}

void Actor::enable()
{
	setEnabled(true);
}

void Actor::disable()
{
	setEnabled(false);
}

bool Actor::canRender() const
{
	return isEnabled();
}

QMatrix4x4 Actor::modelMatrix() const
{
	return _modelMatrix;
}

void Actor::setModelMatrix(const QMatrix4x4& modelMatrix)
{
	if (modelMatrix == _modelMatrix)
		return;

	_modelMatrix = modelMatrix;

	//qDebug() << "Set model > view matrix for" << _name;

	emit modelMatrixChanged(_modelMatrix);
}

QVector3D Actor::translation() const
{
	const auto translationColumn = _modelMatrix.column(3);

	return QVector3D(translationColumn.x(), translationColumn.y(), translationColumn.z());
}

void Actor::setTranslation(const QVector3D& translation)
{
	_modelMatrix.setColumn(3, QVector4D(translation.x(), translation.y(), translation.z(), 1.f));
}

QMatrix4x4 Actor::modelViewMatrix() const
{
	return _renderer->viewMatrix() * _modelMatrix;
}

QMatrix4x4 Actor::modelViewProjectionMatrix() const
{
	return _renderer->projectionMatrix() * modelViewMatrix();
}

bool Actor::mayProcessMousePressEvent() const
{
	if (!shouldReceiveMousePressEvents())
		return false;

	return _enabled && _active;
}

bool Actor::mayProcessMouseReleaseEvent() const
{
	if (!shouldReceiveMouseReleaseEvents())
		return false;

	return _enabled && _active;
}

bool Actor::mayProcessMouseMoveEvent() const
{
	if (!shouldReceiveMouseMoveEvents())
		return false;

	return _enabled && _active;
}

bool Actor::mayProcessMouseWheelEvent() const
{
	if (!shouldReceiveMouseWheelEvents())
		return false;

	return _enabled && _active;
}

bool Actor::mayProcessKeyPressEvent() const
{
	if (!shouldReceiveKeyPressEvents())
		return false;

	return _enabled && _active;
}

bool Actor::mayProcessKeyReleaseEvent() const
{
	if (!shouldReceiveKeyReleaseEvents())
		return false;

	return _enabled && _active;
}

void Actor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event in" << _name;
}

void Actor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event in" << _name;
}

void Actor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event in" << _name;
}

void Actor::onMouseWheelEvent(QWheelEvent* wheelEvent)
{
	//qDebug() << "Mouse wheel event in" << _name;
}

void Actor::onKeyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event in" << _name;
}

void Actor::onKeyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event in" << _name;
}

bool Actor::shouldReceiveMousePressEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::MousePress);
}

bool Actor::shouldReceiveMouseReleaseEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::MouseRelease);
}

bool Actor::shouldReceiveMouseMoveEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::MouseMove);
}

bool Actor::shouldReceiveMouseWheelEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::MouseWheel);
}

bool Actor::shouldReceiveKeyPressEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::KeyPress);
}

bool Actor::shouldReceiveKeyReleaseEvents() const
{
	return _registeredEvents & static_cast<int>(ActorEvent::KeyRelease);
}

bool Actor::isActive() const
{
	return _active;
}

void Actor::activate()
{
	if (!_enabled)
		return;

	_active = true;

	qDebug() << "Activated" << _name;
}

void Actor::deactivate()
{
	_active = false;

	qDebug() << "Deactivated" << _name;
}

float Actor::opacity() const
{
	return _opacity;
}

void Actor::setOpacity(const float& opacity)
{
	if (opacity == _opacity)
		return;

	_opacity = opacity;

	qDebug() << "Set opacity to" << QString::number(opacity, 'f', 2) << "for" << _name;

	emit opacityChanged(_opacity);

	emit changed(this);
}

void Actor::bindOpenGLContext()
{
	_renderer->bindOpenGLContext();
}

void Actor::releaseOpenGLContext()
{
	_renderer->releaseOpenGLContext();
}

Renderer* Actor::renderer()
{
	return _renderer;
}