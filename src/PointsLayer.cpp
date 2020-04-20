#include "PointsLayer.h"
#include "ImageViewerPlugin.h"
#include "PointsProp.h"


#include "PointData.h"
#include "ImageData/Images.h"

#include <QDebug>

PointsLayer::PointsLayer(const QString& pointsDatasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(pointsDatasetName, LayerNode::Type::Points, id, name, flags),
	_pointsDataset(nullptr),
	_imagesDataset(nullptr),
	_channels(),
	_maxNoChannels(0),
	_noChannels(1),
	_colorMap()
{
	init();
}

void PointsLayer::init()
{
	_channels << new Channel(this, 0, 0) << new Channel(this, 1, 1) << new Channel(this, 2, 2);

	addProp<PointsProp>(this, "Points");

	_pointsDataset = &imageViewerPlugin->requestData<Points>(_name);
	_imagesDataset = imageViewerPlugin->sourceImagesSetFromPointsSet(_datasetName);

	//LayerNode::imageViewerPlugin->requestData(_imagesDatasetName);
	setNoPoints(_pointsDataset->getNumPoints());
	setNoDimensions(_pointsDataset->getNumDimensions());
	setMaxNoChannels(std::min(3u, _noDimensions));
	setNoChannels(1);
	setChannelDimensionId(0, 0);

	auto dimensionNames = QStringList::fromVector(QVector<QString>::fromStdVector(_pointsDataset->getDimensionNames()));

	if (dimensionNames.isEmpty()) {
		for (int dimensionIndex = 0; dimensionIndex < noDimensions(Qt::EditRole).toInt(); dimensionIndex++) {
			dimensionNames << QString("Dim %1").arg(dimensionIndex);
		}
	}

	setDimensionNames(dimensionNames);

	const auto pointsDataName = hdps::DataSet::getSourceData(*_pointsDataset).getDataName();

	auto selection = dynamic_cast<Points*>(&imageViewerPlugin->core()->requestSelection(pointsDataName));

	if (selection)
		setSelection(Indices::fromStdVector(selection->indices));
}

Qt::ItemFlags PointsLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::ImageSize:
			break;

		case Column::Channel1:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Channel2:
		{
			if (_maxNoChannels >= 2 && _noChannels >= 2)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3:
		{
			if (_maxNoChannels >= 3 && _noChannels >= 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::MaxNoChannels:
		case Column::NoChannels:
		case Column::DimensionNames:
		case Column::NoPoints:
		case Column::NoDimensions:
			break;

		case Column::ColorMap:
			flags |= Qt::ItemIsEditable;
			break;

		default:
			break;
	}

	return flags;
}

QVariant PointsLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::ImageSize:
			return imageSize(role);

		case Column::Channel1:
			return channelDimensionId(0, role);

		case Column::Channel2:
			return channelDimensionId(1, role);

		case Column::Channel3:
			return channelDimensionId(2, role);

		case Column::MaxNoChannels:
			return maxNoChannels(role);

		case Column::NoChannels:
			return noChannels(role);

		case Column::DimensionNames:
			return dimensionNames(role);
			
		case Column::NoPoints:
			return noPoints(role);

		case Column::NoDimensions:
			return noDimensions(role);

		case Column::ColorMap:
			return colorMap(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	switch (static_cast<Column>(index.column())) {
		case Column::ImageSize:
			break;

		case Column::Channel1:
			setChannelDimensionId(0, value.toInt());
			break;

		case Column::Channel2:
			setChannelDimensionId(1, value.toInt());
			break;

		case Column::Channel3:
			setChannelDimensionId(2, value.toInt());
			break;

		case Column::MaxNoChannels:
			setMaxNoChannels(value.toInt());
			break;

		case Column::NoChannels:
			setNoChannels(value.toInt());
			break;

		case Column::NoPoints:
		case Column::NoDimensions:
			break;

		case Column::ColorMap:
			setColorMap(value.value<QImage>());
			break;

		default:
			break;
	}

	return affectedIds;
}

QVariant PointsLayer::imageSize(const int& role /*= Qt::DisplayRole*/) const
{
	return _imagesDataset->imageSize();
}

QVariant PointsLayer::noPoints(const int& role /*= Qt::DisplayRole*/) const
{
	const auto noPointsString = QString::number(_noPoints);

	switch (role)
	{
		case Qt::DisplayRole:
			return noPointsString;

		case Qt::EditRole:
			return _noPoints;

		case Qt::ToolTipRole:
			return QString("No. points: %1").arg(noPointsString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setNoPoints(const std::uint32_t& noPoints)
{
	_noPoints = noPoints;
}

QVariant PointsLayer::noDimensions(const int& role /*= Qt::DisplayRole*/) const
{
	const auto noDimensionsString = QString::number(_noDimensions);

	switch (role)
	{
		case Qt::DisplayRole:
			return noDimensionsString;

		case Qt::EditRole:
			return _noDimensions;

		case Qt::ToolTipRole:
			return QString("No. dimensions: %1").arg(noDimensionsString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setNoDimensions(const std::uint32_t& noDimensions)
{
	_noDimensions = noDimensions;
}

QVariant PointsLayer::dimensionNames(const int& role /*= Qt::DisplayRole*/) const
{
	const auto imageNamesString = abbreviatedStringList(_dimensionNames);

	switch (role)
	{
		case Qt::DisplayRole:
			return imageNamesString;

		case Qt::EditRole:
			return _dimensionNames;

		case Qt::ToolTipRole:
			return QString("Image names: %1").arg(imageNamesString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setDimensionNames(const QStringList& dimensionNames)
{
	_dimensionNames = dimensionNames;
}

Channel* PointsLayer::channel(const std::uint32_t& id)
{
	return _channels[id];
}

QVariant PointsLayer::channelDimensionId(const std::uint32_t& id, const int& role /*= Qt::DisplayRole*/) const
{
	const auto dimensionIdString = QString::number(_channels[id]->dimensionId());

	switch (role)
	{
		case Qt::DisplayRole:
			return dimensionIdString;

		case Qt::EditRole:
			return _channels[id]->dimensionId();

		case Qt::ToolTipRole:
			return QString("Dimension identifier: %1").arg(dimensionIdString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelDimensionId(const int& channelId, const std::uint32_t& dimensionId)
{
	_channels[channelId]->setDimensionId(dimensionId);

	computeChannel(channelId);
}

QVariant PointsLayer::maxNoChannels(const int& role /*= Qt::DisplayRole*/) const
{
	const auto maxNoChannelsString = QString::number(_maxNoChannels);

	switch (role)
	{
		case Qt::DisplayRole:
			return maxNoChannelsString;

		case Qt::EditRole:
			return _maxNoChannels;

		case Qt::ToolTipRole:
			return QString("Maximum number of channels: %1").arg(maxNoChannelsString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setMaxNoChannels(const std::uint32_t& maxNoChannels)
{
	_maxNoChannels = maxNoChannels;
}

QVariant PointsLayer::noChannels(const int& role /*= Qt::DisplayRole*/) const
{
	return _noChannels;
}

void PointsLayer::setNoChannels(const std::uint32_t& noChannels)
{
	_noChannels = noChannels;

	for (int channelId = 0; channelId < _noChannels; ++channelId)
	{
		computeChannel(channelId);
	}
}

QVariant PointsLayer::colorMap(const int& role) const
{
	const auto colorMapString = "Image";

	switch (role)
	{
		case Qt::DisplayRole:
			return colorMapString;

		case Qt::EditRole:
			return _colorMap;

		case Qt::ToolTipRole:
			return QString("%1").arg(colorMapString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;

	emit colorMapChanged(_colorMap);
}

void PointsLayer::computeChannel(const std::uint32_t& id)
{
	const auto size = imageSize(Qt::EditRole).toSize();

	_channels[id]->setImageSize(size);

	for (int x = 0; x < size.width(); x++)
	{
		for (int y = 0; y < size.height(); y++)
		{
			const auto pixelIndex = y * size.width() + x;

			(*_channels[id])[pixelIndex] = _pointsDataset->getData()[pixelIndex * _pointsDataset->getNumDimensions() + _channels[0]->dimensionId()];
		}
	}

	_channels[id]->setChanged();
}