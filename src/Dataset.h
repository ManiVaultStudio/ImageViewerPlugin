#pragma once

#include "Layer.h"

#include <QString>

class Layer;

/** TODO */
class Dataset
{
public:
	/** TODO */
	Dataset(Layer* layer, const QString& name);

private:
	Layer*		_layer;		/** TODO */
	QString		_name;		/** TODO */
};