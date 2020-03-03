#include "ImageDataset.h"
#include "LayersModel.h"

#include <QDebug>

ImageDataset::ImageDataset(QObject* parent) :
	QObject(parent),
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
	_layers()
{
	addLayer("Image", Layer::Type::Image, true, true);
	addLayer("Selection", Layer::Type::Selection, true, true);
	addLayer("MetaDataA", Layer::Type::Metadata, true, false);
	addLayer("MetaDataB", Layer::Type::Metadata, true, false);
	addLayer("MetaDataC", Layer::Type::Metadata, true, false);
}

void ImageDataset::addLayer(const QString& name, const Layer::Type& type, const bool& enabled, const bool& fixed)
{
	_layers.append(new Layer(this, name, type, enabled, fixed, _layers.size()));
}