#include "PointsLayer.h"
#include "ImageViewerPlugin.h"
#include "PointsProp.h"
#include "Renderer.h"

#include "PointData.h"
#include "ImageData/Images.h"
#include "util/Timer.h"

#include <QDebug>
#include <QImage>

PointsLayer::PointsLayer(const QString& pointsDatasetName, const QString& id, const QString& name, const int& flags) :
	Layer(pointsDatasetName, Layer::Type::Points, id, name, flags),
	Channels<float>(ult(ChannelIndex::Count)),
	_pointsDataset(nullptr),
	_imagesDataset(nullptr),
	_maxNoChannels(0),
	_colorSpace(ColorSpace::RGB),
	_colorMap(),
	_useConstantColor(false),
	_constantColor(Qt::green)
{
	init();
}

void PointsLayer::init()
{
	addProp<PointsProp>(this, "Points");

	_pointsDataset = &imageViewerPlugin->requestData<Points>(_datasetName);
	_imagesDataset = imageViewerPlugin->sourceImagesSetFromPointsSet(_datasetName);

	setNoPoints(_pointsDataset->getNumPoints());
	setNoDimensions(_pointsDataset->getNumDimensions());
	setMaxNoChannels(std::min(3u, _noDimensions));
	setChannelDimensionId(ChannelIndex::Channel1, 0);
	setColorMap(imageViewerPlugin->colorMapModel().colorMap(0)->image());
	setUseConstantColor(false);
	setChannelEnabled(ChannelIndex::Channel1, true);

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

	computeChannel(ChannelIndex::Mask);
}

void PointsLayer::matchScaling(const QSize& targetImageSize)
{
	const auto layerImageSize	= QSizeF(imageSize());
	const auto widthScaling		= static_cast<float>(targetImageSize.width()) / layerImageSize.width();
	const auto heightScaling	= static_cast<float>(targetImageSize.height()) / layerImageSize.height();

	const auto scale = std::min(widthScaling, heightScaling);

	setScale(scale);
}

void PointsLayer::paint(QPainter* painter)
{
	Layer::paint(painter);
}

Qt::ItemFlags PointsLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::Channel1Name:
		case Column::Channel1DimensionId:
		{
			flags |= Qt::ItemIsEditable;
			break;
		}

		case Column::Channel2Name:
		case Column::Channel2DimensionId:
		{
			if (!_useConstantColor && channel(1)->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Name:
		case Column::Channel3DimensionId:
		{
			if (!_useConstantColor && channel(2)->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel1Enabled:
			break;

		case Column::Channel2Enabled:
		{
			if (!_useConstantColor && channel(0)->enabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Enabled:
		{
			if (!_useConstantColor && channel(1)->enabled() && _noDimensions >= 3)
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
			
		case Column::UseConstantColor:
		{
			flags |= Qt::ItemIsEditable;
			break;
		}

		case Column::ConstantColor:
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
		return Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::Channel1Name:
			return channelName(ChannelIndex::Channel1, role);

		case Column::Channel2Name:
			return channelName(ChannelIndex::Channel2, role);

		case Column::Channel3Name:
			return channelName(ChannelIndex::Channel3, role);

		case Column::Channel1DimensionId:
			return channelDimensionId(ChannelIndex::Channel1, role);

		case Column::Channel2DimensionId:
			return channelDimensionId(ChannelIndex::Channel2, role);

		case Column::Channel3DimensionId:
			return channelDimensionId(ChannelIndex::Channel3, role);

		case Column::Channel1Enabled:
			return channelEnabled(ChannelIndex::Channel1, role);

		case Column::Channel2Enabled:
			return channelEnabled(ChannelIndex::Channel2, role);

		case Column::Channel3Enabled:
			return channelEnabled(ChannelIndex::Channel3, role);

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

		case Column::UseConstantColor:
			return useConstantColor(role);

		case Column::ConstantColor:
			return constantColor(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = Layer::setData(index, value, role);

	switch (static_cast<Column>(index.column())) {
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
			setChannelDimensionId(ChannelIndex::Channel1, value.toInt());
			break;

		case Column::Channel2DimensionId:
			setChannelDimensionId(ChannelIndex::Channel2, value.toInt());
			break;

		case Column::Channel3DimensionId:
			setChannelDimensionId(ChannelIndex::Channel3, value.toInt());
			break;

		case Column::Channel1Enabled:
			setChannelEnabled(ChannelIndex::Channel1, value.toBool());
			break;

		case Column::Channel2Enabled:
		{
			const auto enabled = value.toBool();

			setChannelEnabled(ChannelIndex::Channel2, enabled);

			if (enabled == false)
				setChannelEnabled(ChannelIndex::Channel3, enabled);

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

			setChannelEnabled(ChannelIndex::Channel3, enabled);
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

		case Column::UseConstantColor:
		{
			setUseConstantColor(value.toBool());
			setChannelEnabled(ChannelIndex::Channel2, false);
			setChannelEnabled(ChannelIndex::Channel3, false);
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

			if (_useConstantColor) {
				setConstantColor(_constantColor);
			}

			break;
		}

		case Column::ConstantColor:
		{
			setConstantColor(value.value<QColor>());
			
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));

			break;
		}

		default:
			break;
	}

	return affectedIds;
}

QSize PointsLayer::imageSize() const
{
	return _imagesDataset->imageSize();
}

Layer::Hints PointsLayer::hints() const
{
	return Layer::hints();
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

QVariant PointsLayer::channelEnabled(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto channelEnabled		= channel(ult(channelIndex))->enabled();
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

void PointsLayer::setChannelEnabled(const ChannelIndex& channelIndex, const bool& enabled)
{
	channel(ult(channelIndex))->setEnabled(enabled);
}

QVariant PointsLayer::channelName(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto nameString = channel(ult(channelIndex))->name();

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
	channel(id)->setName(name);
}

QVariant PointsLayer::channelDimensionId(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto dimensionIdString = QString::number(channel(ult(channelIndex))->dimensionId());

	switch (role)
	{
		case Qt::DisplayRole:
			return dimensionIdString;

		case Qt::EditRole:
			return channel(ult(channelIndex))->dimensionId();

		case Qt::ToolTipRole:
			return QString("Dimension identifier: %1").arg(dimensionIdString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelDimensionId(const ChannelIndex& channelIndex, const std::uint32_t& dimensionId)
{
	channel(ult(channelIndex))->setDimensionId(dimensionId);

	computeChannel(channelIndex);
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
		channel(0)->enabled(),
		channel(1)->enabled(),
		channel(2)->enabled()
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

QVariant PointsLayer::useConstantColor(const int& role) const
{
	const auto useConstantColorString = _useConstantColor ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return useConstantColorString;

		case Qt::EditRole:
			return _useConstantColor;

		case Qt::ToolTipRole:
			return QString("Use constant color: %1").arg(useConstantColorString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setUseConstantColor(const bool& useConstantColor)
{
	_useConstantColor = useConstantColor;
}

QVariant PointsLayer::constantColor(const int& role) const
{
	const auto constantColorString = QString("rgb(%1, %2, %3)").arg(QString::number(_constantColor.red()), QString::number(_constantColor.green()), QString::number(_constantColor.blue()));

	switch (role)
	{
		case Qt::DisplayRole:
			return constantColorString;

		case Qt::EditRole:
			return _constantColor;

		case Qt::ToolTipRole:
			return QString("Constant color: %1").arg(constantColorString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setConstantColor(const QColor& constantColor)
{
	_constantColor = constantColor;
}

void PointsLayer::computeChannel(const ChannelIndex& channelIndex)
{
#ifdef _DEBUG
	auto timer = Timer("Compute channel");
#endif

	switch (channelIndex)
	{
		case ChannelIndex::Channel1:
		case ChannelIndex::Channel2:
		case ChannelIndex::Channel3:
		{
			auto channel = this->channel(ult(channelIndex));

			if (channel->dimensionId() < 0)
				return;

			channel->setImageSize(imageSize());

			if (_pointsDataset->isDerivedData()) {
				auto& sourceData = _pointsDataset->getSourceData<Points>(*_pointsDataset);

				if (sourceData.isFull()) {
					for (int i = 0; i < _pointsDataset->getNumPoints(); i++) {
						(*channel)[i] = _pointsDataset->getData()[i * _noDimensions + channel->dimensionId()];
					}
				}
				else {
					for (int i = 0; i < sourceData.indices.size(); i++) {
						(*channel)[sourceData.indices[i]] = _pointsDataset->getData()[i * _noDimensions + channel->dimensionId()];
					}
				}
			}
			else {
				if (_pointsDataset->isFull()) {
					for (int i = 0; i < noPixels(); i++) {
						(*channel)[i] = _pointsDataset->getData()[i * _noDimensions + channel->dimensionId()];
					}
				}
				else {
					for (const auto& index : _pointsDataset->indices) {
						(*channel)[index] = _pointsDataset->getData()[index * _noDimensions + channel->dimensionId()];
					}
				}
			}

			channel->setChanged();

			emit channelChanged(ult(channelIndex));

			break;
		}

		case ChannelIndex::Mask:
		{
			auto maskChannel = channel(ult(ChannelIndex::Mask));

			maskChannel->setImageSize(imageSize());

			if (_pointsDataset->isDerivedData()) {
				auto& sourceData = _pointsDataset->getSourceData<Points>(*_pointsDataset);

				if (sourceData.isFull()) {
					maskChannel->fill(1.0f);
				}
				else {
					for (auto index : sourceData.indices) {
						(*maskChannel)[index] = 1.0f;
					}
				}
			}
			else {
				if (_pointsDataset->isFull()) {
					maskChannel->fill(1.0f);
				}
				else {
					for (auto index : _pointsDataset->indices) {
						(*maskChannel)[index] = 1.0f;
					}
				}
			}

			maskChannel->setChanged();

			emit channelChanged(ult(channelIndex));

			break;
		}

		default:
			break;
	}
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

			case ColorSpace::LAB:
				setChannelName(0, "L");
				setChannelName(1, "A");
				setChannelName(2, "B");
				break;

			default:
				break;
		}
	}
}