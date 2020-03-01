#include "ImageDataset.h"

#include <QDebug>

ImageDataset::ImageDataset() :
	_name(""),
	_type(0),
	_noImages(0),
	_size(),
	_noPoints(0),
	_noDimensions(0),
	_currentImage(-1),
	_currentDimension(-1),
	_imageNames(),
	_dimensionNames(),
	_averageImages(),
	_imageFilePaths(),
	_layers(),
	_layersModel(&_layers)
{
	_layers.append(Layer("Image", Layer::Type::Image, true, 0));
	_layers.append(Layer("Selection", Layer::Type::Selection, true, 1));
	_layers.append(Layer("MetaA", Layer::Type::Metadata, true, 2));
	_layers.append(Layer("MetaB", Layer::Type::Metadata, true, 3));
}