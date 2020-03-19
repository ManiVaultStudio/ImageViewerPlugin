#pragma once

#include "Actor.h"

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

protected: // TODO

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles = QVector<int>());

	void render();
private:
	LayersModel*	_layersModel;		/** TODO */
};