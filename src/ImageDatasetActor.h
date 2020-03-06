#pragma once

#include "Actor.h"

#include "ImageDataset.h"

class Renderer;
class LayersModel;

/**
 * Layer actor class
 * @author Thomas Kroes
 */
class ImageDatasetActor : public Actor
{
	Q_OBJECT

public:
	/** TODO */
	ImageDatasetActor(Renderer* renderer, const QString& name, LayersModel* layersModel, const bool& visible = true);

protected: // Layer prop management

	/** TODO */
	void addLayerProp(const QString& layerName);

	/** TODO */
	void removeLayerProp(const QString& layerName);

private:
	LayersModel*	_layersModel;		/** TODO */
};