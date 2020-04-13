#include "RenderNode.h"
#include "Renderer.h"

#include <QMouseEvent>
#include <QDebug>

Renderer* RenderNode::renderer = nullptr;

RenderNode::RenderNode(const QString& id, const QString& name, const int& flags) :
	Node(id, name, flags),
	_registeredEvents(static_cast<int>(ActorEvent::None)),
	_mouseEvents(),
	_opacity(1.0f),
	_modelMatrix(),
	_props()
{
}

RenderNode::~RenderNode() = default;

void RenderNode::render()
{
	if (!isRenderable())
		return;

	qDebug() << "Render" << _name;

	for (auto name : _props.keys()) {
		_props[name]->render();
	}
}

QMatrix4x4 RenderNode::modelMatrix() const
{
	return _modelMatrix;
}

void RenderNode::setModelMatrix(const QMatrix4x4& modelMatrix)
{
	if (modelMatrix == _modelMatrix)
		return;

	_modelMatrix = modelMatrix;
}

QMatrix4x4 RenderNode::modelViewMatrix() const
{
	return renderer->viewMatrix() * _modelMatrix;
}

QMatrix4x4 RenderNode::modelViewProjectionMatrix() const
{
	return renderer->projectionMatrix() * modelViewMatrix();
}

void RenderNode::registerMousePressEvents()
{
	QObject::connect(renderer, &Renderer::mousePressEvent, [this](QMouseEvent* mouseEvent) {
		if (isEnabled() && isRenderable())
			onMousePressEvent(mouseEvent);
	});
}

void RenderNode::registerMouseReleaseEvents()
{
	QObject::connect(renderer, &Renderer::mouseReleaseEvent, [this](QMouseEvent* mouseEvent) {
		if (isEnabled() && isRenderable())
			onMouseReleaseEvent(mouseEvent);
	});
}

void RenderNode::registerMouseMoveEvents()
{
	QObject::connect(renderer, &Renderer::mouseMoveEvent, [this](QMouseEvent* mouseEvent) {
		if (isEnabled() && isRenderable())
			onMouseMoveEvent(mouseEvent);
	});
}

void RenderNode::registerMouseWheelEvents()
{
	QObject::connect(renderer, &Renderer::mouseWheelEvent, [this](QWheelEvent* wheelEvent) {
		if (isEnabled() && isRenderable())
			onMouseWheelEvent(wheelEvent);
	});
}

void RenderNode::registerKeyPressEvents()
{
	QObject::connect(renderer, &Renderer::keyPressEvent, [this](QKeyEvent* keyEvent) {
		if (isEnabled() && isRenderable())
			onKeyPressEvent(keyEvent);
	});
}

void RenderNode::registerKeyReleaseEvents()
{
	QObject::connect(renderer, &Renderer::keyReleaseEvent, [this](QKeyEvent* keyEvent) {
		if (isEnabled() && isRenderable())
			onKeyReleaseEvent(keyEvent);
	});
}

void RenderNode::onMousePressEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse press event in" << _name;
}

void RenderNode::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse release event in" << _name;
}

void RenderNode::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	qDebug() << "Mouse move event in" << _name;
}

void RenderNode::onMouseWheelEvent(QWheelEvent* wheelEvent)
{
	qDebug() << "Mouse wheel event in" << _name;
}

void RenderNode::onKeyPressEvent(QKeyEvent* keyEvent)
{
	qDebug() << "Key press event in" << _name;
}

void RenderNode::onKeyReleaseEvent(QKeyEvent* keyEvent)
{
	qDebug() << "Key release event in" << _name;
}

void RenderNode::addMouseEvent(QMouseEvent* mouseEvent)
{
	const auto screenPoint		= QVector2D(mouseEvent->pos());
	const auto worldPosition	= renderer->screenPointToWorldPosition(modelViewMatrix(), screenPoint);

	_mouseEvents.append(MouseEvent(screenPoint, worldPosition));
}

QVector<RenderNode::MouseEvent> RenderNode::mouseEvents()
{
	return _mouseEvents;
}

QVariant RenderNode::opacity(const int& role) const
{
	const auto opacityString = QString("%1%").arg(QString::number(100.0f * _opacity, 'f', 1));

	switch (role)
	{
		case Qt::DisplayRole:
			return opacityString;

		case Qt::EditRole:
			return _opacity;

		case Qt::ToolTipRole:
			return QString("Opacity: %1").arg(opacityString);

		default:
			break;
	}

	return QVariant();
}

void RenderNode::setOpacity(const float& opacity)
{
	_opacity = opacity;
}