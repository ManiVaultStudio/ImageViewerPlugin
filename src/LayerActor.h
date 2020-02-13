#pragma once

#include "Actor.h"

class Renderer;
class Layer;

/**
 * Layer actor class
 * @author Thomas Kroes
 */
class LayerActor : public Actor
{
	Q_OBJECT

public:
	LayerActor(Renderer* renderer, const QString& name);

	/**
	 * Sets the layer
	 * @param layer Layer
	 */
	void setLayer(Layer* layer);

protected: // Event handlers
	
	/**
	 * Invoked when the layer changed
	 * @param image Layer image
	 */
	void onImageChanged(const QImage& image);

private:
	Layer*		_layer;		/** Layer */
};