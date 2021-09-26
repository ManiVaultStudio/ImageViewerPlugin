#include "Renderable.h"
#include "Renderer.h"

#include <QDebug>

Renderable::Renderable(Renderer& renderer) :
    _renderer(renderer),
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
    return _modelMatrix;
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
