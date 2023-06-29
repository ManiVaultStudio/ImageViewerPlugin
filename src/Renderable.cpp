#include "Renderable.h"
#include "LayersRenderer.h"

#include <QDebug>

Renderable::Renderable(LayersRenderer* renderer /*= nullptr*/) :
    _renderer(renderer),
    _modelMatrix(),
    _props()
{
}

LayersRenderer* Renderable::getRenderer()
{
    return _renderer;
}

const LayersRenderer* Renderable::getRenderer() const
{
    return _renderer;
}

void Renderable::setRenderer(LayersRenderer* layersRenderer)
{
    _renderer = layersRenderer;
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
    if (_renderer == nullptr)
        return {};

    return _renderer->getViewMatrix() * _modelMatrix;
}

QMatrix4x4 Renderable::getModelViewProjectionMatrix() const
{
    if (_renderer == nullptr)
        return {};

    return _renderer->getProjectionMatrix() * getModelViewMatrix();
}
