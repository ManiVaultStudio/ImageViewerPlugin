#include "ClustersLayer.h"
#include "Dataset.h"

#include <QDebug>

ClustersLayer::ClustersLayer(Dataset* dataset, const QString& id /*= ""*/, const QString& name /*= ""*/, const std::uint32_t& flags /*= Flags::Enabled*/) :
	Layer(dataset, id, name, Layer::Type::Clusters, flags)
{
}