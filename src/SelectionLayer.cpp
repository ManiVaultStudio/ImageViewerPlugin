#include "SelectionLayer.h"
#include "ImageViewerPlugin.h"

#include "PointData.h"

#include <QDebug>

SelectionLayer::SelectionLayer(const QString& datasetName, const QString& id, const QString& name, const int& flags) :
	LayerNode(datasetName, LayerNode::Type::Selection, id, name, flags),
	_image()
{
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return QVariant();
}

QModelIndexList SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	if (static_cast<LayerNode::Column>(index.column()) == LayerNode::Column::Selection) {
		
		
	}

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return affectedIds;
}

QVariant SelectionLayer::image(const int& role) const
{
	const auto imageString = "SelectionImage";

	switch (role)
	{
		case Qt::DisplayRole:
			return imageString;

		case Qt::EditRole:
			return _image;

		case Qt::ToolTipRole:
			return QString("%1").arg(imageString);

		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setImage(const QImage& image)
{
	_image= image;

	emit imageChanged(_image);
}

void SelectionLayer::computeImage()
{
	/*
	auto points = dynamic_cast<Points*>(LayerNode::imageViewerPlugin->requestData<Points>(_datasetName));

	auto& selection = dynamic_cast<Points&>(_core->requestSelection(_imageData->points()->getDataName()));

	const auto noElements = _imageData->noImages() * _imageData->noComponents();
	const auto width = imageSize().width();
	const auto height = imageSize().height();

	auto imageData = std::vector<std::uint8_t>();

	imageData.resize(noPixels() * 4);

	

	const auto imageDataWidth = _imageData->imageSize().width();

	for (const auto& selectionId : selection.indices)
	{
		const auto x = selectionId % imageDataWidth;
		const auto y = static_cast<std::uint32_t>(floorf(static_cast<float>(selectionId) / static_cast<float>(imageDataWidth)));

		if (_roi.contains(x, y)) {
			const auto pixelId = ((y - _roi.top()) * width) + (x - _roi.left());
			const auto pixelOffset = pixelId * 4;

			imageData[pixelOffset + 0] = 255;
			imageData[pixelOffset + 1] = 255;
			imageData[pixelOffset + 2] = 255;
			imageData[pixelOffset + 3] = 255;
		}
	}

	auto image = QImage(width, height, QImage::Format::Format_RGB32);

	memcpy(image.bits(), imageData.data(), imageData.size() * sizeof(std::uint8_t));

	return image;
	*/
}