#pragma once

#include "Actor.h"

class Renderer;
class LayersModel;

/**
 * Layer actor class
 * 
 * Class for rendering a layer on the screen
 * @author Thomas Kroes
 */
class LayerActor : public Actor
{
	Q_OBJECT

public:
	/** TODO */
	LayerActor(Actor* actor, const QString& name, LayersModel* layersModel, const bool& visible = true);

public: // 

	void render();

private:
	LayersModel*	_layersModel;		/** TODO */
};