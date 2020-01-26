#include "StackedRenderer.h"

#include <QDebug>

StackedRenderer::StackedRenderer(const float& depth) :
	Renderer(),
	_depth(depth),
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