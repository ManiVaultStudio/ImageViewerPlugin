#include "LayerActor.h"
#include "ImagesProp.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QDebug>

LayerActor::LayerActor(Actor* actor, const QString& name, LayersModel* layersModel, const bool& visible /*= true*/) :
	Actor(actor, name, visible),
	_layersModel(layersModel)
{
}

void LayerActor::render()
{
}