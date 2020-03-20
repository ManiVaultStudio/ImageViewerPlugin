#include "PointsLayer.h"
#include "Dataset.h"

#include <QDebug>

PointsLayer::PointsLayer(Dataset* dataset, const QString& id /*= ""*/, const QString& name /*= ""*/, const std::uint32_t& flags /*= Flags::Enabled*/) :
	Layer(dataset, id, name, Layer::Type::Image, flags)
{
}