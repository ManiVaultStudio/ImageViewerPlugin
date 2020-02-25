#pragma once

#include "Actor.h"
#include "ImageLayer.h"

class Renderer;
class ImageDataset;

/**
 * Layer actor class
 * @author Thomas Kroes
 */
class ImageDatasetActor : public Actor
{
	Q_OBJECT

public:
	ImageDatasetActor(Renderer* renderer, const QString& name, const bool& visible = true);

	/** TODO */
	void setDataset(ImageDataset* dataset);

protected: // Dataset linking

	/** TODO */
	void connectToDataset();

	/** TODO */
	void disconnectFromDataset();

protected: // Layer prop management

	/** TODO */
	void addLayerProp(const QString& layerName);

	/** TODO */
	void removeLayerProp(const QString& layerName);

private:
	ImageDataset*		_dataset;		/** Layer */
};