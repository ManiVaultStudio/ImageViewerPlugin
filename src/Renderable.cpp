#include "Renderable.h"
#include "Renderer.h"

#include <QDebug>

Renderable::Renderable(Renderer& renderer) :
    _renderer(renderer),
    _opacity(1.0f),
    _scale(1.0f),
    _modelMatrix(),
    _props()
{
}

Renderer& Renderable::getRenderer()
{
    return _renderer;
}

QMatrix4x4 Renderable::getModelMatrix() const
{
    auto scaleMatrix = QMatrix4x4();

    scaleMatrix.scale(_scale);

    return _modelMatrix * scaleMatrix;
}

void Renderable::setModelMatrix(const QMatrix4x4& modelMatrix)
{
    if (modelMatrix == _modelMatrix)
        return;

    _modelMatrix = modelMatrix;
}

QMatrix4x4 Renderable::getModelViewMatrix() const
{
    return _renderer.getViewMatrix() * _modelMatrix;
}

QMatrix4x4 Renderable::getModelViewProjectionMatrix() const
{
    return _renderer.getProjectionMatrix() * getModelViewMatrix();
}

float Renderable::getOpacity() const
{
    return _opacity;
}

void Renderable::setOpacity(const float& opacity)
{
    _opacity = opacity;
}

float Renderable::getScale() const
{
    return _scale;
}

void Renderable::setScale(const float& scale)
{
    _scale = scale;
}
