#pragma once

#include "Actor.h"

class Renderer;
class ImageDataset;
class ImageLayer;

/**
 * Layer actor class
 * @author Thomas Kroes
 */
class DatasetActor : public Actor
{
	Q_OBJECT

public:
	DatasetActor(Renderer* renderer, const QString& name);

	/**
	 * Sets the layer
	 * @param layer Layer
	 */
	void setDataset(ImageDataset* dataset);

protected: // Dataset linking

	/**
	 * 
	 */
	void connectToDataset(ImageDataset* dataset);

	/**
	 * 
	 */
	void disconnectFromDataset(ImageDataset* dataset);

protected: // Event handlers

	/**
	 * 
	 */
	void onAddLayer(ImageLayer* layer);

	/**
	 * 
	 */
	void onRemoveLayer(ImageLayer* layer);

protected: // Layer prop management

	/**
	 * 
	 */
	void addLayerProp(ImageLayer* layer);

	/**
	 * 
	 */
	void removeLayerProp(ImageLayer* layer);

private:
	ImageDataset*		_dataset;		/** Layer */
};