#include "ImagesLayer.h"
#include "Dataset.h"

#include <QDebug>

ImagesLayer::ImagesLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags) :
	Layer(dataset, id, name, Layer::Type::Images, flags)
{
}