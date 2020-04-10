#include "Actor.h"

#include <QMouseEvent>
#include <QDebug>

#include "Renderer.h"

Actor::Actor(Actor* parent, const QString& name, const bool& visible /*= true*/) :
	QObject(nullptr),
	_registeredEvents(static_cast<int>(ActorEvent::None)),
	_mouseEvents(),
	_name(name),
	_enabled(true),
	_visible(visible),
	_opacity(1.0f),
	_modelMatrix(),
	_props()
{
}

Actor::~Actor() = default;

void Actor::destroy()
{
	//qDebug() << "Destroy" << _name;

	bindOpenGLContext();
	
	for (auto name : _props.keys()) {
		_props[name]->destroy();
	}
}

void Actor::render()
{
	if (!canRender())
		return;

	//qDebug() << "Render" << _name;

	for (auto name : _props.keys()) {
		_props[name]->render();
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

	qDebug() << "Set " << _name << (_enabled ? "enabled" : "disabled");
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
	return isVisible();
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
}

QMatrix4x4 Actor::modelViewMatrix() const
{
	return renderer()->viewMatrix() * _modelMatrix;
}

QMatrix4x4 Actor::modelViewProjectionMatrix() const
{
	return renderer()->projectionMatrix() * modelViewMatrix();
}

bool Actor::mayProcessMousePressEvent() const
{
	if (!shouldReceiveMousePressEvents())
		return false;

	return _enabled && _visible;
}

bool Actor::mayProcessMouseReleaseEvent() const
{
	if (!shouldReceiveMouseReleaseEvents())
		return false;

	return _enabled && _visible;
}

bool Actor::mayProcessMouseMoveEvent() const
{
	if (!shouldReceiveMouseMoveEvents())
		return false;

	return _enabled && _visible;
}

bool Actor::mayProcessMouseWheelEvent() const
{
	if (!shouldReceiveMouseWheelEvents())
		return false;

	return _enabled && _visible;
}

bool Actor::mayProcessKeyPressEvent() const
{
	if (!shouldReceiveKeyPressEvents())
		return false;

	return _enabled && _visible;
}

bool Actor::mayProcessKeyReleaseEvent() const
{
	if (!shouldReceiveKeyReleaseEvents())
		return false;

	return _enabled && _visible;
}

QVector<Actor::MouseEvent> Actor::mouseEvents()
{
	return _mouseEvents;
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

void Actor::addMouseEvent(QMouseEvent* mouseEvent)
{
	const auto screenPoint		= QVector2D(mouseEvent->pos());
	const auto worldPosition	= renderer()->screenPointToWorldPosition(modelViewMatrix(), screenPoint);

	_mouseEvents.append(MouseEvent(screenPoint, worldPosition));
}

bool Actor::isVisible() const
{
	return _visible;
}

void Actor::show()
{
	if (!_enabled)
		return;

	_visible = true;

	qDebug() << "Show" << _name;
}

void Actor::hide()
{
	_visible = false;

	qDebug() << "Hide" << _name;
}

void Actor::setVisible(const bool& visible)
{
	if (visible)
		show();
	else
		hide();
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
}

void Actor::bindOpenGLContext()
{
	renderer()->bindOpenGLContext();
}

void Actor::releaseOpenGLContext()
{
	renderer()->releaseOpenGLContext();
}

const Renderer* Actor::renderer() const
{
	return static_cast<Renderer*>(parent());
}

Renderer* Actor::renderer()
{
	const auto constThis = const_cast<const Actor*>(this);
	return const_cast<Renderer*>(constThis->renderer());
}