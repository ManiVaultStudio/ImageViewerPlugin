#include "Renderable.h"
#include "Renderer.h"

#include <QDebug>

Renderer* Renderable::renderer = nullptr;

Renderable::Renderable() :
	_mouseEvents(),
	_opacity(1.0f),
	_modelMatrix(),
	_props()
{
}

Renderable::~Renderable() = default;

QMatrix4x4 Renderable::modelMatrix() const
{
	return _modelMatrix;
}

void Renderable::setModelMatrix(const QMatrix4x4& modelMatrix)
{
	if (modelMatrix == _modelMatrix)
		return;

	_modelMatrix = modelMatrix;
}

QMatrix4x4 Renderable::modelViewMatrix() const
{
	return renderer->viewMatrix() * _modelMatrix;
}

QMatrix4x4 Renderable::modelViewProjectionMatrix() const
{
	return renderer->projectionMatrix() * modelViewMatrix();
}

QVariant Renderable::opacity(const int& role) const
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

void Renderable::setOpacity(const float& opacity)
{
	_opacity = opacity;
}