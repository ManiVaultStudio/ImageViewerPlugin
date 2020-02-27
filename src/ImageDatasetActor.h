#pragma once

#include "Actor.h"

#include "ImageDataset.h"

class Renderer;

/**
 * Layer actor class
 * @author Thomas Kroes
 */
class ImageDatasetActor : public Actor
{
	Q_OBJECT

public:
	/** TODO */
	ImageDatasetActor(Renderer* renderer, const QString& name, ImageDataset* imageDataset, const bool& visible = true);

protected: // Layer prop management

	/** TODO */
	void addLayerProp(const QString& layerName);

	/** TODO */
	void removeLayerProp(const QString& layerName);

private:
	ImageDataset*	_imageDataset;		/** Layer */
};