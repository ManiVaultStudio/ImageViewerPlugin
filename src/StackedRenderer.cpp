#include "StackedRenderer.h"

#include <QDebug>

StackedRenderer::StackedRenderer(const float& depth) :
	Renderer(),
	_depth(depth),
	_modelViewProjection(),
	_fbos()
{
}

void StackedRenderer::init()
{
	initializeOpenGLFunctions();
}

float StackedRenderer::depth() const
{
	return _depth;
}

void StackedRenderer::setDepth(const float& depth)
{
	if (depth == _depth)
		return;

	_depth = depth;
}

QMatrix4x4 StackedRenderer::modelViewProjection() const
{
	return _modelViewProjection;
}

void StackedRenderer::setModelViewProjection(const QMatrix4x4& modelViewProjection)
{
	_modelViewProjection = modelViewProjection;

	_modelViewProjection.translate(0.f, 0.f, static_cast<float>(_depth));
}