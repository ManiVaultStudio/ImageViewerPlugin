#pragma once

#include "Actor.h"

class Layer;

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
	LayerActor(Layer* layer, const QString& name);

public: // 

	void render();

private:
	Layer*	_layer;		/** TODO */
};