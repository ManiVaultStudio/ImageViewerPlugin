#include "PointsLayer.h"
#include "ImageViewerPlugin.h"
#include "PointsProp.h"
#include "Renderer.h"

#include "PointData.h"
#include "ImageData/Images.h"

#include <QDebug>

PointsLayer::PointsLayer(const QString& pointsDatasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(pointsDatasetName, LayerNode::Type::Points, id, name, flags),
	_pointsDataset(nullptr),
	_imagesDataset(nullptr),
	_channels(),
	_maxNoChannels(0),
	_colorSpace(ColorSpace::RGB),
	_colorMap()
{
	init();
}

void PointsLayer::init()
{
	_channels << new Channel(this, 0, "Channel 1") << new Channel(this, 1, "Channel 2") << new Channel(this, 2, "Channel 3");
	
	addProp<PointsProp>(this, "Points");

	_pointsDataset = &imageViewerPlugin->requestData<Points>(_name);
	_imagesDataset = imageViewerPlugin->sourceImagesSetFromPointsSet(_datasetName);

	setNoPoints(_pointsDataset->getNumPoints());
	setNoDimensions(_pointsDataset->getNumDimensions());
	setMaxNoChannels(std::min(3u, _noDimensions));
	setChannelDimensionId(0, 0);
	setColorMap(imageViewerPlugin->colorMapModel().colorMap(0)->image());
	setSolidColor(false);
	setChannelEnabled(0, true);

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

		case Column::Channel1Name:
		case Column::Channel1DimensionId:
		{
			flags |= Qt::ItemIsEditable;
			break;
		}

		case Column::Channel2Name:
		case Column::Channel2DimensionId:
		{
			if (!_solidColor && _channels[1]->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Name:
		case Column::Channel3DimensionId:
		{
			if (!_solidColor && _channels[2]->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel1Enabled:
			break;

		case Column::Channel2Enabled:
		{
			if (!_solidColor && _channels[0]->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Enabled:
		{
			if (!_solidColor && _channels[1]->enabled() && _noDimensions >= 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::MaxNoChannels:
		case Column::NoChannels:
		case Column::DimensionNames:
		case Column::NoPoints:
		case Column::NoDimensions:
			break;

		case Column::ColorSpace:
		{
			if (noChannels(Qt::EditRole).toInt() == 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::ColorMap:
		{
			if (noChannels(Qt::EditRole).toInt() < 3)
				flags |= Qt::ItemIsEditable;

			break;
		}
			
		case Column::SolidColor:
		{
			flags |= Qt::ItemIsEditable;
			break;
		}

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

		case Column::Channel1Name:
			return channelName(0, role);

		case Column::Channel2Name:
			return channelName(1, role);

		case Column::Channel3Name:
			return channelName(2, role);

		case Column::Channel1DimensionId:
			return channelDimensionId(0, role);

		case Column::Channel2DimensionId:
			return channelDimensionId(1, role);

		case Column::Channel3DimensionId:
			return channelDimensionId(2, role);

		case Column::Channel1Enabled:
			return channelEnabled(0, role);

		case Column::Channel2Enabled:
			return channelEnabled(1, role);

		case Column::Channel3Enabled:
			return channelEnabled(2, role);

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

		case Column::ColorSpace:
			return colorSpace(role);

		case Column::ColorMap:
			return colorMap(role);

		case Column::SolidColor:
			return solidColor(role);

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

		case Column::Channel1Name:
			setChannelName(0, value.toString());
			break;

		case Column::Channel2Name:
			setChannelName(1, value.toString());
			break;

		case Column::Channel3Name:
			setChannelName(2, value.toString());
			break;

		case Column::Channel1DimensionId:
			setChannelDimensionId(0, value.toInt());
			break;

		case Column::Channel2DimensionId:
			setChannelDimensionId(1, value.toInt());
			break;

		case Column::Channel3DimensionId:
			setChannelDimensionId(2, value.toInt());
			break;

		case Column::Channel1Enabled:
			setChannelEnabled(0, value.toBool());
			break;

		case Column::Channel2Enabled:
		{
			const auto enabled = value.toBool();

			setChannelEnabled(1, enabled);

			if (enabled == false)
				setChannelEnabled(2, enabled);

			affectedIds << index.siblingAtColumn(ult(Column::Channel2DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::ColorSpace));
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));
			break;
		}

		case Column::Channel3Enabled:
		{
			const auto enabled = value.toBool();

			setChannelEnabled(2, enabled);
			updateChannelNames();

			affectedIds << index.siblingAtColumn(ult(Column::Channel1Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::ColorSpace));
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));
			break;
		}

		case Column::MaxNoChannels:
			setMaxNoChannels(value.toInt());
			break;

		case Column::NoChannels:
			break;

		case Column::NoPoints:
		case Column::NoDimensions:
			break;

		case Column::ColorSpace:
		{
			const auto colorSpace = static_cast<ColorSpace>(value.toInt());

			setColorSpace(colorSpace);
			updateChannelNames();

			affectedIds << index.siblingAtColumn(ult(Column::Channel1Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Name));
			break;
		}

		case Column::ColorMap:
			setColorMap(value.value<QImage>());
			break;

		case Column::SolidColor:
			setSolidColor(value.toBool());
			setChannelEnabled(1, false);
			setChannelEnabled(2, false);
			updateChannelNames();

			affectedIds << index.siblingAtColumn(ult(Column::Channel1Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Name));
			affectedIds << index.siblingAtColumn(ult(Column::Channel1DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel1Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::ColorSpace));
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));
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

QVariant PointsLayer::channelEnabled(const std::uint32_t& id, const int& role /*= Qt::DisplayRole*/) const
{
	const auto channelEnabled		= _channels[id]->enabled();
	const auto channelEnabledString = channelEnabled ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return channelEnabledString;

		case Qt::EditRole:
			return channelEnabled;

		case Qt::ToolTipRole:
			return QString("Enabled: %1").arg(channelEnabledString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelEnabled(const int& id, const bool& enabled)
{
	_channels[id]->setEnabled(enabled);
}

QVariant PointsLayer::channelName(const std::uint32_t& id, const int& role /*= Qt::DisplayRole*/) const
{
	const auto nameString = _channels[id]->name();

	switch (role)
	{
		case Qt::DisplayRole:
			return nameString;

		case Qt::EditRole:
			return nameString;

		case Qt::ToolTipRole:
			return QString("Name: %1").arg(nameString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelName(const int& id, const QString& name)
{
	_channels[id]->setName(name);
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

void PointsLayer::setChannelDimensionId(const int& id, const std::uint32_t& dimensionId)
{
	_channels[id]->setDimensionId(dimensionId);

	computeChannel(id);
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
	QVector<int> channels = {
		_channels[0]->enabled(),
		_channels[1]->enabled(),
		_channels[2]->enabled()
	};

	return std::accumulate(channels.begin(), channels.end(), 0);
}

QVariant PointsLayer::colorSpace(const int& role) const
{
	const auto colorSpaceString = colorSpaceName(_colorSpace);

	switch (role)
	{
		case Qt::DisplayRole:
			return colorSpaceString;

		case Qt::EditRole:
			return ult(_colorSpace);

		case Qt::ToolTipRole:
			return QString("Color space: %1").arg(colorSpaceString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setColorSpace(const ColorSpace& colorSpace)
{
	_colorSpace = colorSpace;

	emit colorSpaceChanged(_colorSpace);
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

QVariant PointsLayer::solidColor(const int& role) const
{
	const auto solidColorString = _solidColor ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return solidColorString;

		case Qt::EditRole:
			return _solidColor;

		case Qt::ToolTipRole:
			return QString("Solid color: %1").arg(solidColorString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setSolidColor(const bool& solidColor)
{
	_solidColor = solidColor;
}

void PointsLayer::computeChannel(const std::uint32_t& id)
{
	if (_channels[id]->dimensionId() < 0)
		return;

	const auto size = imageSize(Qt::EditRole).toSize();

	_channels[id]->setImageSize(size);

	for (int x = 0; x < size.width(); x++)
	{
		for (int y = 0; y < size.height(); y++)
		{
			const auto pixelIndex = y * size.width() + x;

			(*_channels[id])[pixelIndex] = _pointsDataset->getData()[pixelIndex * _pointsDataset->getNumDimensions() + _channels[id]->dimensionId()];
		}
	}

	_channels[id]->setChanged();
}

void PointsLayer::updateChannelNames()
{
	if (noChannels(Qt::EditRole).toInt() < 3) {
		setChannelName(0, "Channel 1");
		setChannelName(1, "Channel 2");
		setChannelName(2, "Channel 3");
	}
	else {
		switch (_colorSpace)
		{
			case ColorSpace::RGB:
				setChannelName(0, "Red");
				setChannelName(1, "Green");
				setChannelName(2, "Blue");
				break;

			case ColorSpace::HSL:
				setChannelName(0, "Hue");
				setChannelName(1, "Saturation");
				setChannelName(2, "Lightness");
				break;

			default:
				break;
		}
	}
}