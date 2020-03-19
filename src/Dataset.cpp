#include "Dataset.h"
#include "Layer.h"

Dataset::Dataset(Layer* layer, const QString& name) :
	_layer(layer),
	_name(name)
{
}