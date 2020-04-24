#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"
#include "SelectionProp.h"

#include "PointData.h"
#include "ImageData/Images.h"

#include <QDebug>

SelectionLayer::SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(datasetName, LayerNode::Type::Selection, id, name, flags),
	_pointsDataset(nullptr),
	_imagesDataset(nullptr),
	_image(),
	_imageData(),
	_colorMap()
{
	init();
}

void SelectionLayer::init()
{
	addProp<SelectionProp>(this, "Selection");

	_pointsDataset	= &imageViewerPlugin->requestData<Points>(_datasetName);
	_imagesDataset	= imageViewerPlugin->sourceImagesSetFromPointsSet(_datasetName);
	_dataName		= hdps::DataSet::getSourceData(*_pointsDataset).getDataName();

	//setColorMap(imageViewerPlugin->colorMapModel().colorMap(0)->image());

	computeImage();
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::ColorMap:
			flags |= Qt::ItemIsEditable;
			break;

		default:
			break;
	}


	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::ColorMap:
			return colorMap(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	if (static_cast<LayerNode::Column>(index.column()) == LayerNode::Column::Selection) {
		computeImage();
	}

	switch (static_cast<Column>(index.column())) {
		case Column::ColorMap:
			setColorMap(value.value<QImage>());
			break;

		default:
			break;
	}

	return affectedIds;
}

QVariant SelectionLayer::colorMap(const int& role) const
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

void SelectionLayer::setColorMap(const QImage& colorMap)
{
	_colorMap = colorMap;

	emit colorMapChanged(_colorMap);
}

void SelectionLayer::computeImage()
{
	auto points = dynamic_cast<Points*>(&LayerNode::imageViewerPlugin->requestData<Points>(_datasetName));

	if (points == nullptr)
		return;

	const auto imageSize	= _imagesDataset->imageSize();
	const auto width		= imageSize.width();
	const auto height		= imageSize.height();
	const auto noPixels		= width * height;
	const auto noChannels	= 4;
	const auto noElements	= noPixels * noChannels;

	if (noElements !=_imageData.count()) {
		_imageData.resize(noElements);
	}

	_imageData.fill(0, noElements);

	for (const auto& selectionId : _selection)
	{
		const auto x		= selectionId % width;
		const auto y		= static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(width)));
		const auto pixelId	= (y * width) + x;
		const auto offset	= pixelId * noChannels;

		for (int c = 0; c < noChannels; c++)
			_imageData[offset + c] = 255;
	}

	if (_image.isNull() || imageSize != _image.size())
		_image = QImage(width, height, QImage::Format_RGBA8888);

	memcpy(_image.bits(), _imageData.data(), _imageData.size() * sizeof(std::uint8_t));

	emit imageChanged(_image);
}