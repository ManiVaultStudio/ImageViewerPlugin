#include "ImageDatasetActor.h"
#include "ImageLayerProp.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QDebug>

ImageDatasetActor::ImageDatasetActor(Renderer* renderer, const QString& name, LayersModel* layersModel, const bool& visible /*= true*/) :
	Actor(renderer, name, visible),
	_layersModel(layersModel)
{
	for (int row = 0; row < _layersModel->rowCount(); row++) {
		const auto id	= _layersModel->data(row, Layer::Column::ID, Qt::EditRole).toString();
		const auto type	= _layersModel->data(row, Layer::Column::Type, Qt::EditRole).toInt();
		
		addProp<ImageLayerProp>(this, id, static_cast<Layer::Type>(type));
	}

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImageDatasetActor::updateData);
	

	QObject::connect(_layersModel, &LayersModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int last) {
		for (int row = first; row <= last; row++) {
			const auto id	= _layersModel->data(row, Layer::Column::ID, Qt::EditRole).toString();
			const auto type = _layersModel->data(row, Layer::Column::Type, Qt::EditRole).toInt();

			addProp<ImageLayerProp>(this, id, static_cast<Layer::Type>(type));
		}
	});

	updateData(_layersModel->index(0), _layersModel->index(_layersModel->rowCount() - 1, _layersModel->columnCount() - 1));
}

void ImageDatasetActor::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
	for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
		const auto layerId = _layersModel->data(row, Layer::Column::ID, Qt::EditRole).toString();
		const auto layerProp = this->propByName<ImageLayerProp>(layerId);

		if (topLeft.column() <= Layer::columnId(Layer::Column::Enabled) && bottomRight.column() >= Layer::columnId(Layer::Column::Enabled)) {
			const auto enabled = _layersModel->data(row, Layer::Column::Enabled, Qt::EditRole).toBool();
			layerProp->setVisible(enabled);
		}

		if (topLeft.column() <= Layer::columnId(Layer::Column::Image) && bottomRight.column() >= Layer::columnId(Layer::Column::Image)) {
			const auto layerImage = _layersModel->data(row, Layer::Column::Image, Qt::EditRole).value<QImage>();
			layerProp->setImage(layerImage);
		}

		if (topLeft.column() <= Layer::columnId(Layer::Column::DisplayRange) && bottomRight.column() >= Layer::columnId(Layer::Column::DisplayRange)) {
			const auto layerDisplayRange = _layersModel->data(row, Layer::Column::DisplayRange, Qt::EditRole).value<Range>();
			layerProp->setDisplayRange(layerDisplayRange.min(), layerDisplayRange.max());
		}

		if (topLeft.column() <= Layer::columnId(Layer::Column::Opacity) && bottomRight.column() >= Layer::columnId(Layer::Column::Opacity)) {
			const auto layerOpacity = _layersModel->data(row, Layer::Column::Opacity, Qt::EditRole).toFloat();
			layerProp->setOpacity(layerOpacity);
		}

		if (topLeft.column() <= Layer::columnId(Layer::Column::Order) && bottomRight.column() >= Layer::columnId(Layer::Column::Order)) {
			const auto order = _layersModel->data(row, Layer::Column::Order, Qt::EditRole).toInt();
			layerProp->setOrder(order);
		}

		emit becameDirty(this);
	}
}

void ImageDatasetActor::render()
{
	if (!canRender())
		return;

	for (int row = 0; row < _layersModel->rowCount(); row++) {
		const auto layerId = _layersModel->data(_layersModel->rowCount() - 1 - row, Layer::Column::ID, Qt::EditRole).toString();
		this->propByName<ImageLayerProp>(layerId)->render();
	}
}