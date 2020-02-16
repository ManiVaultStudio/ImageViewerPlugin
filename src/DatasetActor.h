#pragma once

#include "Actor.h"

class Renderer;
class Dataset;
class Layer;

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
	void setDataset(Dataset* dataset);

protected: // Dataset linking

	/**
	 * 
	 */
	void connectToDataset(Dataset* dataset);

	/**
	 * 
	 */
	void disconnectFromDataset(Dataset* dataset);

protected: // Event handlers

	/**
	 * 
	 */
	void onAddLayer(Layer* layer);

	/**
	 * 
	 */
	void onRemoveLayer(Layer* layer);

protected: // Layer prop management

	/**
	 * 
	 */
	void addLayerProp(Layer* layer);

	/**
	 * 
	 */
	void removeLayerProp(Layer* layer);

private:
	Dataset*		_dataset;		/** Layer */
};