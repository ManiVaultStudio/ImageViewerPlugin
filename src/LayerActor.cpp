#include "LayerActor.h"
#include "ImagesProp.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QDebug>

LayerActor::LayerActor(Layer* layer, const QString& name) :
	Actor(nullptr, name),
	_layer(layer)
{
}

void LayerActor::render()
{
}