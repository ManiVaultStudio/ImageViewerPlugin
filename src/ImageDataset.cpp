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
	_pointsName(),
	_selection(),
	_layers(),
	_layersModel(QSharedPointer<LayersModel>::create(&_layers))
{
	addLayer("Image", Layer::Type::Image, Layer::Flags::Enabled | Layer::Flags::Fixed);
	addLayer("Selection", Layer::Type::Selection, Layer::Flags::Enabled | Layer::Flags::Fixed);
	addLayer("MetaDataA", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable);
	addLayer("MetaDataB", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable);
	addLayer("MetaDataC", Layer::Type::Metadata, Layer::Flags::Enabled | Layer::Flags::Removable);
}

void ImageDataset::addLayer(const QString& name, const Layer::Type& type, const std::uint32_t& flags)
{
	_layers.append(new Layer(this, name, type, flags, _layers.size()));
}