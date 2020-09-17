#include "PointsLayer.h"
#include "ImageViewerPlugin.h"
#include "PointsProp.h"
#include "Renderer.h"

#include "PointData.h"
#include "util/Timer.h"

#include "ImageData/Images.h"

#include <QDebug>
//#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QTextDocument>

#include <set>

PointsLayer::PointsLayer(const QString& pointsDatasetName, const QString& id, const QString& name, const int& flags) :
	Layer(pointsDatasetName, Layer::Type::Points, id, name, flags),
	Channels<float>(ult(ChannelIndex::Count)),
	_pointsDataset(nullptr),
	_maxNoChannels(0),
	_colorSpace(ColorSpace::RGB),
	_colorMap(),
	_useConstantColor(false),
	_constantColor(Qt::green),
	_pointType(PointType::Intensity),
	_indexSelectionDatasetName(),
	_indexSelectionDataset(nullptr)
{
	init();

	QObject::connect(imageViewerPlugin, &ImageViewerPlugin::selectionIndicesChanged, [this](const QString& name) {
		if (_pointType == PointType::Index && _indexSelectionDataset != nullptr) {
			if (name == _indexSelectionDataset->getDataName()) {
				computeChannel(ChannelIndex::Channel1);
				computeChannel(ChannelIndex::Mask);

				Renderable::renderer->render();
			}
		}
	});
}

void PointsLayer::init()
{
	addProp<PointsProp>(this, "Points");

	_pointsDataset	= &imageViewerPlugin->requestData<Points>(_datasetName);
	_dataName		= hdps::DataSet::getSourceData(*_pointsDataset).getDataName();

	setNoPoints(_pointsDataset->getNumPoints());
	setNoDimensions(_pointsDataset->getNumDimensions());
	setMaxNoChannels(std::min(3u, _noDimensions));
	setColorMap(imageViewerPlugin->getColorMapModel().getColorMap(0)->getImage());
	setUseConstantColor(false);
	setChannelEnabled(ChannelIndex::Channel1, true);

	auto dimensionNames = hdps::fromStdVector<QStringList>(_pointsDataset->getDimensionNames());

	if (dimensionNames.isEmpty()) {
		for (int dimensionIndex = 0; dimensionIndex < getNoDimensions(Qt::EditRole).toInt(); dimensionIndex++) {
			dimensionNames << QString("Dim %1").arg(dimensionIndex);
		}
	}

	setDimensionNames(dimensionNames);

	setChannelDimensionId(ChannelIndex::Channel1, 0);
	setChannelDimensionId(ChannelIndex::Channel2, std::min(1, dimensionNames.count() - 1));
	setChannelDimensionId(ChannelIndex::Channel3, std::min(2, dimensionNames.count() - 1));

	const auto pointsDataName = hdps::DataSet::getSourceData(*_pointsDataset).getDataName();

	auto selection = dynamic_cast<Points*>(&imageViewerPlugin->core()->requestSelection(pointsDataName));

	if (selection) {
		setSelection(hdps::fromStdVector<QVector<std::uint32_t>>(selection->indices));
	}

	computeChannel(ChannelIndex::Mask);
}

void PointsLayer::matchScaling(const QSize& targetImageSize)
{
	const auto layerImageSize	= QSizeF(getImageSize());
	const auto widthScaling		= static_cast<float>(targetImageSize.width()) / layerImageSize.width();
	const auto heightScaling	= static_cast<float>(targetImageSize.height()) / layerImageSize.height();

	const auto scale = std::min(widthScaling, heightScaling);

	setScale(scale);
}

void PointsLayer::paint(QPainter* painter)
{
	Layer::paint(painter);

	if (!isFlagSet(Node::Flag::Enabled) || renderer->interactionMode() != InteractionMode::LayerEditing)
		return;

	if (_mousePositions.isEmpty())
		return;

	//painter->drawPoint(_mousePositions.last());

	/*
	const auto textAngle = 0.75f * M_PI;
	const auto size = 12.0f;
	const auto textCenter = brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));

	textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));
	

	//renderer->
	painter->setPen(Qt::SolidLine);
	painter->setBrush(Qt::red);
	painter->setPen(Qt::SolidLine);
	painter->drawText(_mousePositions.last(), u8"Value:");//_mousePositions.last()

	

	QTextDocument hintsDocument;

	QString hintsHtml;

	const auto color = QString("rgba(%1, %2, %3, %4)").arg(QString::number(hintsColor.red()), QString::number(hintsColor.green()), QString::number(hintsColor.blue()), QString::number(isFlagSet(Flag::Enabled) ? hintsColor.alpha() : 80));

	hintsHtml += QString("<div><table style='color: %1;'>").arg(color);

	hintsHtml += QString("<tr><td>%1</td><td>%2</td></tr>").arg("Channel 1", "10.0");

	hintsHtml += "</table></div>";

	hintsDocument.setTextWidth(200);
	//hintsDocument.setDocumentMargin(textMargins);
	hintsDocument.setHtml(hintsHtml);
	hintsDocument.drawContents(painter, QRect(_mousePositions.last(), QSize(1000, 1000)));
	*/
}

void PointsLayer::handleEvent(QEvent* event, const QModelIndex& index)
{
	if (renderer->interactionMode() != InteractionMode::LayerEditing)
		return;

	switch (event->type())
	{
		// Prevent recursive paint events
		case QEvent::Paint:
			return;

		case QEvent::MouseButtonPress:
			break;

		case QEvent::MouseButtonRelease:
			break;

		case QEvent::MouseMove:
		{
			auto mouseEvent = static_cast<QMouseEvent*>(event);

			const auto mousePosition = mouseEvent->pos();

			if (_mousePositions.isEmpty())
				_mousePositions << mousePosition;

			_mousePositions[0] = mousePosition;

			break;
		}

		case QEvent::Wheel:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
			break;

		default:
			break;
	}

	Renderable::renderer->render();
}

Qt::ItemFlags PointsLayer::getFlags(const QModelIndex& index) const
{
	auto flags = Layer::getFlags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::DimensionNames:
			break;

		case Column::Channel1Name:
		case Column::Channel1DimensionId:
		{
			if (_pointType == PointType::Intensity)
				flags |= Qt::ItemIsEditable;
			break;
		}

		case Column::Channel2Name:
		case Column::Channel2DimensionId:
		{
			if (_pointType == PointType::Intensity && !_useConstantColor && getChannel(1)->getEnabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Name:
		case Column::Channel3DimensionId:
		{
			if (_pointType == PointType::Intensity && !_useConstantColor && getChannel(2)->getEnabled())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel1Enabled:
			break;

		case Column::Channel2Enabled:
		{
			if (_pointType == PointType::Intensity && !_useConstantColor && getChannel(0)->getEnabled() && _noDimensions > 1)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3Enabled:
		{
			if (_pointType == PointType::Intensity && !_useConstantColor && getChannel(1)->getEnabled() && _noDimensions >= 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::MaxNoChannels:
		case Column::NoChannels:
		case Column::NoPoints:
		case Column::NoDimensions:
			break;

		case Column::ColorSpace:
		{
			if (_pointType == PointType::Intensity && getNoChannels(Qt::EditRole).toInt() == 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::ColorMap:
		{
			if (getNoChannels(Qt::EditRole).toInt() < 3)
				flags |= Qt::ItemIsEditable;

			break;
		}
			
		case Column::UseConstantColor:
		{
			if (_pointType == PointType::Intensity)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::ConstantColor:
		{
			flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::PointType:
			break;

		case Column::IndexSelectionDatasetName:
		{
			if (_pointType == PointType::Index)
				flags |= Qt::ItemIsEditable;

			break;
		}

		default:
			break;
	}

	return flags;
}

QVariant PointsLayer::getData(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return Layer::getData(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::DimensionNames:
			return getDimensionNames(role);

		case Column::Channel1Name:
			return getChannelName(ChannelIndex::Channel1, role);

		case Column::Channel2Name:
			return getChannelName(ChannelIndex::Channel2, role);

		case Column::Channel3Name:
			return getChannelName(ChannelIndex::Channel3, role);

		case Column::Channel1DimensionId:
			return getChannelDimensionId(ChannelIndex::Channel1, role);

		case Column::Channel2DimensionId:
			return getChannelDimensionId(ChannelIndex::Channel2, role);

		case Column::Channel3DimensionId:
			return getChannelDimensionId(ChannelIndex::Channel3, role);

		case Column::Channel1Enabled:
			return getChannelEnabled(ChannelIndex::Channel1, role);

		case Column::Channel2Enabled:
			return getChannelEnabled(ChannelIndex::Channel2, role);

		case Column::Channel3Enabled:
			return getChannelEnabled(ChannelIndex::Channel3, role);

		case Column::Channel1Window:
			return getChannelWindow(ChannelIndex::Channel1, role);

		case Column::Channel2Window:
			return getChannelWindow(ChannelIndex::Channel2, role);

		case Column::Channel3Window:
			return getChannelWindow(ChannelIndex::Channel3, role);

		case Column::Channel1Level:
			return getChannelLevel(ChannelIndex::Channel1, role);

		case Column::Channel2Level:
			return getChannelLevel(ChannelIndex::Channel2, role);

		case Column::Channel3Level:
			return getChannelLevel(ChannelIndex::Channel3, role);

		case Column::MaxNoChannels:
			return getMaxNoChannels(role);

		case Column::NoChannels:
			return getNoChannels(role);
			
		case Column::NoPoints:
			return getNoPoints(role);

		case Column::NoDimensions:
			return getNoDimensions(role);

		case Column::ColorSpace:
			return getColorSpace(role);

		case Column::ColorMap:
			return getColorMap(role);

		case Column::UseConstantColor:
			return getUseConstantColor(role);

		case Column::ConstantColor:
			return getConstantColor(role);

		case Column::PointType:
			return getPointType(role);

		case Column::IndexSelectionDatasetName:
			return getIndexSelectionDatasetName(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = Layer::setData(index, value, role);

	if (static_cast<Layer::Column>(index.column()) == Layer::Column::Selection) {
		computeChannel(ChannelIndex::Channel1);
	}

	switch (static_cast<Column>(index.column())) {
		case Column::DimensionNames:
			break;

		case Column::Channel1Name:
		{
			setChannelName(0, value.toString());
			break;
		}

		case Column::Channel2Name:
		{
			setChannelName(1, value.toString());
			break;
		}

		case Column::Channel3Name:
		{
			setChannelName(2, value.toString());
			break;
		}

		case Column::Channel1DimensionId:
		{
			setChannelDimensionId(ChannelIndex::Channel1, value.toInt());
			break;
		}

		case Column::Channel2DimensionId:
		{
			setChannelDimensionId(ChannelIndex::Channel2, value.toInt());
			break;
		}

		case Column::Channel3DimensionId:
		{
			setChannelDimensionId(ChannelIndex::Channel3, value.toInt());
			break;
		}

		case Column::Channel1Enabled:
		{
			setChannelEnabled(ChannelIndex::Channel1, value.toBool());
			break;
		}

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

		case Column::Channel1Window:
		{
			setChannelWindow(ChannelIndex::Channel1, value.toFloat());
			break;
		}

		case Column::Channel2Window:
		{
			setChannelWindow(ChannelIndex::Channel2, value.toFloat());
			break;
		}

		case Column::Channel3Window:
		{
			setChannelWindow(ChannelIndex::Channel3, value.toFloat());
			break;
		}

		case Column::Channel1Level:
		{
			setChannelLevel(ChannelIndex::Channel1, value.toFloat());
			break;
		}

		case Column::Channel2Level:
		{
			setChannelLevel(ChannelIndex::Channel2, value.toFloat());
			break;
		}

		case Column::Channel3Level:
		{
			setChannelLevel(ChannelIndex::Channel3, value.toFloat());
			break;
		}

		case Column::MaxNoChannels:
		{
			setMaxNoChannels(value.toInt());
			break;
		}

		case Column::NoChannels:
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
		{
			setColorMap(value.value<QImage>());
			break;
		}

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

			if (_useConstantColor)
				setConstantColor(_constantColor);

			break;
		}

		case Column::ConstantColor:
		{
			setConstantColor(value.value<QColor>());
			
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));

			break;
		}

		case Column::PointType:
		{
			setPointType(static_cast<PointType>(value.toInt()));
			
			affectedIds << index.siblingAtColumn(ult(Column::IndexSelectionDatasetName));
			affectedIds << index.siblingAtColumn(ult(Column::Channel1DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3DimensionId));
			affectedIds << index.siblingAtColumn(ult(Column::Channel1Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::Channel2Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::Channel3Enabled));
			affectedIds << index.siblingAtColumn(ult(Column::ColorSpace));
			affectedIds << index.siblingAtColumn(ult(Column::ColorMap));
			affectedIds << index.siblingAtColumn(ult(Column::UseConstantColor));
			affectedIds << index.siblingAtColumn(ult(Column::ConstantColor));

			break;
		}

		case Column::IndexSelectionDatasetName:
		{
			setIndexSelectionDatasetName(value.toString());
			break;
		}

		default:
			break;
	}

	return affectedIds;
}

QSize PointsLayer::getImageSize() const
{
	if (_pointsDataset->isDerivedData()) {
		auto sourcePointsDataset = hdps::DataSet::getSourceData<Points>(*_pointsDataset);

		return sourcePointsDataset.getProperty("ImageSize", QSize()).toSize();
	}

	return _pointsDataset->getProperty("ImageSize", QSize()).toSize();
}

std::int32_t PointsLayer::getImageCollectionType() const
{
	if (_pointsDataset->isDerivedData()) {
		auto sourcePointsDataset = hdps::DataSet::getSourceData<Points>(*_pointsDataset);

		return sourcePointsDataset.getProperty("CollectionType", ImageData::Type::Undefined).toInt();
	}

	return _pointsDataset->getProperty("CollectionType", ImageData::Type::Undefined).toInt();
}

Layer::Hints PointsLayer::getHints() const
{
	return Layer::getHints();
}

QVariant PointsLayer::getNoPoints(const int& role /*= Qt::DisplayRole*/) const
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

QVariant PointsLayer::getNoDimensions(const int& role /*= Qt::DisplayRole*/) const
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

QVariant PointsLayer::getDimensionNames(const int& role /*= Qt::DisplayRole*/) const
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

QVariant PointsLayer::getChannelEnabled(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto channelEnabled		= getChannel(ult(channelIndex))->getEnabled();
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
	getChannel(ult(channelIndex))->setEnabled(enabled);
}

QVariant PointsLayer::getChannelWindow(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto channelWindow		= getChannel(ult(channelIndex))->getWindowNormalized();
	const auto channelWindowString	= QString::number(channelWindow, 'f', 1);

	switch (role)
	{
		case Qt::DisplayRole:
			return channelWindowString;

		case Qt::EditRole:
			return channelWindow;

		case Qt::ToolTipRole:
			return QString("Normalized window: %1").arg(channelWindowString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelWindow(const ChannelIndex& channelIndex, const float& window)
{
	getChannel(ult(channelIndex))->setWindowNormalized(window);
}

QVariant PointsLayer::getChannelLevel(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto channelLevel			= getChannel(ult(channelIndex))->getLevelNormalized();
	const auto channelLevelString	= QString::number(channelLevel, 'f', 1);

	switch (role)
	{
		case Qt::DisplayRole:
			return channelLevelString;

		case Qt::EditRole:
			return channelLevel;

		case Qt::ToolTipRole:
			return QString("Normalized level: %1").arg(channelLevelString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelLevel(const ChannelIndex& channelIndex, const float& level)
{
	getChannel(ult(channelIndex))->setLevelNormalized(level);
}

QVariant PointsLayer::getChannelName(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto nameString = getChannel(ult(channelIndex))->getName();

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
	getChannel(id)->setName(name);
}

QVariant PointsLayer::getChannelDimensionId(const ChannelIndex& channelIndex, const int& role /*= Qt::DisplayRole*/) const
{
	const auto dimensionIdString = QString::number(getChannel(ult(channelIndex))->getDimensionId());

	switch (role)
	{
		case Qt::DisplayRole:
			return dimensionIdString;

		case Qt::EditRole:
			return getChannel(ult(channelIndex))->getDimensionId();

		case Qt::ToolTipRole:
			return QString("Dimension identifier: %1").arg(dimensionIdString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannelDimensionId(const ChannelIndex& channelIndex, const std::uint32_t& dimensionId)
{
	getChannel(ult(channelIndex))->setDimensionId(dimensionId);
	
	computeChannel(channelIndex);

	getChannel(ult(channelIndex))->computeRange();
	getChannel(ult(channelIndex))->computeDisplayRange();
}

QVariant PointsLayer::getMaxNoChannels(const int& role /*= Qt::DisplayRole*/) const
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

QVariant PointsLayer::getNoChannels(const int& role /*= Qt::DisplayRole*/) const
{
	QVector<int> channels = {
		getChannel(0)->getEnabled(),
		getChannel(1)->getEnabled(),
		getChannel(2)->getEnabled()
	};

	return std::accumulate(channels.begin(), channels.end(), 0);
}

QVariant PointsLayer::getColorSpace(const int& role) const
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

QVariant PointsLayer::getColorMap(const int& role) const
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

QVariant PointsLayer::getUseConstantColor(const int& role) const
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

QVariant PointsLayer::getConstantColor(const int& role) const
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

QVariant PointsLayer::getPointType(const int& role /*= Qt::DisplayRole*/) const
{
	const auto pixelTypeString = pixelTypeName(_pointType);

	switch (role)
	{
		case Qt::DisplayRole:
			return pixelTypeString;

		case Qt::EditRole:
			return ult(_pointType);

		case Qt::ToolTipRole:
			return QString("Pixel type: %1").arg(pixelTypeString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setPointType(const PointType& pointType)
{
	_pointType				= pointType;
	_indexSelectionDataset	= _indexSelectionDatasetName.isEmpty() ? nullptr : &imageViewerPlugin->requestData<Points>(_indexSelectionDatasetName);

	switch (_pointType)
	{
		case PointsLayer::PointType::Intensity:
		{
			setUseConstantColor(false);
			break;
		}

		case PointsLayer::PointType::Index:
		{
			setUseConstantColor(true);
			setConstantColor(Qt::red);
			break;
		}

		default:
			break;
	}
	computeChannel(ChannelIndex::Mask);
}

QVariant PointsLayer::getIndexSelectionDatasetName(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
			return _indexSelectionDatasetName;

		case Qt::EditRole:
			return _indexSelectionDatasetName;

		case Qt::ToolTipRole:
			return QString("Name of the index selection dataset: %1").arg(_indexSelectionDatasetName);

		default:
			break;
	}

	return QVariant();
}

QVariant PointsLayer::getIndexSelectionDataName(const int& role /*= Qt::DisplayRole*/) const
{
	if (_indexSelectionDataset == nullptr)
		return "";

	const auto indexSelectionDataName = _indexSelectionDataset->getDataName();

	switch (role)
	{
		case Qt::DisplayRole:
			return indexSelectionDataName;

		case Qt::EditRole:
			return indexSelectionDataName;

		case Qt::ToolTipRole:
			return QString("Name of the index selection raw data: %1").arg(indexSelectionDataName);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setIndexSelectionDatasetName(const QString& indexSelectionDatasetName)
{
	_indexSelectionDatasetName	= indexSelectionDatasetName;
	_indexSelectionDataset		= _indexSelectionDatasetName.isEmpty() ? nullptr : &imageViewerPlugin->requestData<Points>(_indexSelectionDatasetName);

	computeChannel(ChannelIndex::Channel1);
}

QVariant PointsLayer::getIndicesSelection(const int& role /*= Qt::DisplayRole*/) const
{
	/*
	auto selection = QStringList();

	if (_indicesSelection.size() <= 2) {
		for (const auto& id : _indicesSelection)
			selection << QString::number(id);
	}
	else {
		selection << QString::number(_indicesSelection.first());
		selection << "...";
		selection << QString::number(_indicesSelection.last());
	}

	const auto indicesSelectionString = QString("[%1]").arg(selection.join(", "));

	switch (role)
	{
		case Qt::DisplayRole:
			return indicesSelectionString;

		case Qt::EditRole:
			return QVariant::fromValue(_indicesSelection);

		case Qt::ToolTipRole:
			return QString("Indices selection: %1").arg(indicesSelectionString);

		default:
			break;
	}
	*/
	return QVariant();
}

void PointsLayer::computeChannel(const ChannelIndex& channelIndex)
{
#ifdef _DEBUG
	auto timer = Timer("Compute channel");
#endif

	auto channel = this->getChannel(ult(channelIndex));

	channel->setImageSize(getImageSize());

	switch (channelIndex)
	{
		case ChannelIndex::Channel1:
		case ChannelIndex::Channel2:
		case ChannelIndex::Channel3:
		{
			if (channel->getDimensionId() < 0)
				break;

			switch (_pointType)
			{
				case PointsLayer::PointType::Intensity:
				{
					switch (static_cast<ImageData::Type>(getImageCollectionType()))
					{
						case ImageData::Type::Sequence:
						{
							computeSequenceChannel(channel, channelIndex);
							break;
						}

						case ImageData::Type::Stack:
						{
							computeStackChannel(channel, channelIndex);
							break;
						}

						default:
							break;
					}

					break;
				}

				case PointsLayer::PointType::Index:
				{
					/*
					if (channelIndex == ChannelIndex::Channel1) {
						computeIndexChannel(channel, channelIndex);
					}
					*/

					switch (static_cast<ImageData::Type>(getImageCollectionType()))
					{
						case ImageData::Type::Sequence:
						{
							computeSequenceChannel(channel, channelIndex);
							break;
						}

						case ImageData::Type::Stack:
						{
							computeStackChannel(channel, channelIndex);
							break;
						}

						default:
							break;
					}

					break;
				}

				default:
					break;
			}
			

			break;
		}

		case ChannelIndex::Mask:
		{
			computeMaskChannel(channel, channelIndex);
			break;
		}

		default:
			break;
	}
}

void PointsLayer::computeSequenceChannel(Channel<float>* channel, const ChannelIndex& channelIndex)
{
	channel->fill(0.0f);

	_pointsDataset->visitData([this, channel](auto pointData) {
		const auto hasSelection	= _selection.count() > 0;
		const auto dimensionId	= channel->getDimensionId();
		const auto width		= channel->getImageSize().width();
		const auto height		= channel->getImageSize().height();
		const auto noPixels		= width * height;

		if (hasSelection) {
			for (const auto& index : _selection) {
				const auto point	= pointData[index];
				const auto sum		= std::accumulate(point.begin(), point.end(), 0.0);

				(*channel)[index] = static_cast<float>(sum / _selection.count());
			}
		}
		else {
			auto pixelIndex = 0;

			for (auto dataValue : pointData[dimensionId]) {
				(*channel)[pixelIndex] = dataValue;
				pixelIndex++;
				
			}
		}
	});

	channel->setChanged();

	emit channelChanged(ult(channelIndex));
}

void PointsLayer::computeStackChannel(Channel<float>* channel, const ChannelIndex& channelIndex)
{
	channel->fill(0.0f);

	const auto dimensionId = channel->getDimensionId();

	if (_pointsDataset->isDerivedData()) {
		_pointsDataset->visitData([this, channel, dimensionId](auto pointData) {
			auto& sourceData = _pointsDataset->getSourceData<Points>(*_pointsDataset);

			if (sourceData.isFull()) {
				for (unsigned int i = 0; i < _pointsDataset->getNumPoints(); i++)
					(*channel)[i] = pointData[i][dimensionId];
			}
			else {
				for (int i = 0; i < sourceData.indices.size(); i++)
					(*channel)[sourceData.indices[i]] = pointData[i][dimensionId];
			}
		});

		/* This does not work with derived data from subsets
		_pointsDataset->visitData([this, channel](auto pointData) {
			const auto dimensionId = channel->dimensionId();

			for (auto pointView : pointData) {
				(*channel)[_pointsDataset->indices[pointView.index()]] = pointView[dimensionId];
			}
		});
		*/
	}
	else {
		_pointsDataset->visitSourceData([this, channel](auto pointData) {
			const auto dimensionId = channel->getDimensionId();

			for (auto pointView : pointData)
				(*channel)[pointView.index()] = pointView[dimensionId];
		});
	}

	channel->setChanged();

	emit channelChanged(ult(channelIndex));
}

void PointsLayer::computeMaskChannel(Channel<float>* maskChannel, const ChannelIndex& channelIndex)
{
	switch (_pointType)
	{
		case PointsLayer::PointType::Intensity:
		{
			maskChannel->fill(0.0f);

			if (_pointsDataset->isDerivedData()) {
				_pointsDataset->visitData([this, maskChannel](auto pointData) {
					auto& sourceData = _pointsDataset->getSourceData<Points>(*_pointsDataset);

					if (sourceData.isFull()) {
						for (int i = 0; i < _pointsDataset->getNumPoints(); i++)
							(*maskChannel)[i] = 1.0f;
					}
					else {
						for (int i = 0; i < sourceData.indices.size(); i++)
							(*maskChannel)[sourceData.indices[i]] = 1.0f;
					}
				});
			} else {
				_pointsDataset->visitData([this, maskChannel](auto pointData) {
					for (auto pointView : pointData) {
						(*maskChannel)[pointView.index()] = 1.0f;
					}
				});
			}

			break;
		}

		case PointsLayer::PointType::Index:
		{
			maskChannel->fill(0.0f);

			if (_indexSelectionDataset == nullptr)
				break;

			auto& selection = dynamic_cast<Points&>(imageViewerPlugin->core()->requestSelection(_indexSelectionDataset->getDataName()));

			std::vector<bool> selectedElements;

			selectedElements.resize(maskChannel->getElements().size());

			for (const auto& index : selection.indices) {
				selectedElements[index] = 1;
			}

			auto indexChannel = this->getChannel(ult(ChannelIndex::Channel1));

			const auto noElements = indexChannel->getElements().size();

			for (int i = 0; i < noElements; ++i) {
				const auto index = static_cast<std::int32_t>((*indexChannel)[i]);

				if (index > 0)
					(*maskChannel)[i] = selectedElements[index + 1] ? 1 : 0;
				else
					(*maskChannel)[i] = 0.0f;
			}

			break;
		}

		default:
			break;
	}

	maskChannel->setChanged();

	emit channelChanged(ult(channelIndex));
}

void PointsLayer::computeIndexChannel(Channel<float>* channel, const ChannelIndex& channelIndex)
{
	const auto dimensionId = channel->getDimensionId();

	channel->fill(1.0f);

	channel->setChanged();

	emit channelChanged(ult(channelIndex));
}

void PointsLayer::updateChannelNames()
{
	if (getNoChannels(Qt::EditRole).toInt() < 3) {
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