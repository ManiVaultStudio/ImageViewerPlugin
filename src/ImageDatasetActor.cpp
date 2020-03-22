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
		const auto id	= _layersModel->data(row, layerColumnId(LayerColumn::ID), Qt::EditRole).toString();
		const auto type	= _layersModel->data(row, layerColumnId(LayerColumn::Type), Qt::EditRole).toInt();
		
		addProp<ImageLayerProp>(this, id, static_cast<LayerType>(type));
	}

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImageDatasetActor::updateData);
	

	QObject::connect(_layersModel, &LayersModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int last) {
		for (int row = first; row <= last; row++) {
			const auto id	= _layersModel->data(row, layerColumnId(LayerColumn::ID), Qt::EditRole).toString();
			const auto type	= _layersModel->data(row, layerColumnId(LayerColumn::Type), Qt::EditRole).toInt();

			addProp<ImageLayerProp>(this, id, static_cast<LayerType>(type));
		}
	});

	updateData(_layersModel->index(0), _layersModel->index(_layersModel->rowCount() - 1, _layersModel->columnCount() - 1));
}

void ImageDatasetActor::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
	for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
		const auto layerId = _layersModel->data(row, layerColumnId(LayerColumn::ID), Qt::EditRole).toString();
		const auto layerProp = this->propByName<ImageLayerProp>(layerId);

		if (topLeft.column() <= layerColumnId(LayerColumn::Enabled) && bottomRight.column() >= layerColumnId(LayerColumn::Enabled)) {
			const auto enabled = _layersModel->data(row, layerColumnId(LayerColumn::Enabled), Qt::EditRole).toBool();
			layerProp->setVisible(enabled);
		}

		if (topLeft.column() <= layerColumnId(LayerColumn::Image) && bottomRight.column() >= layerColumnId(LayerColumn::Image)) {
			const auto layerImage = _layersModel->data(row, layerColumnId(LayerColumn::Image), Qt::EditRole).value<QImage>();
			layerProp->setImage(layerImage);
		}

		if (topLeft.column() <= layerColumnId(LayerColumn::DisplayRange) && bottomRight.column() >= layerColumnId(LayerColumn::DisplayRange)) {
			const auto layerDisplayRange = _layersModel->data(row, layerColumnId(LayerColumn::DisplayRange), Qt::EditRole).value<Range>();
			layerProp->setDisplayRange(layerDisplayRange.min(), layerDisplayRange.max());
		}

		if (topLeft.column() <= layerColumnId(LayerColumn::Opacity) && bottomRight.column() >= layerColumnId(LayerColumn::Opacity)) {
			const auto layerOpacity = _layersModel->data(row, layerColumnId(LayerColumn::Opacity), Qt::EditRole).toFloat();
			layerProp->setOpacity(layerOpacity);
		}

		if (topLeft.column() <= layerColumnId(LayerColumn::Order) && bottomRight.column() >= layerColumnId(LayerColumn::Order)) {
			const auto order = _layersModel->data(row, layerColumnId(LayerColumn::Order), Qt::EditRole).toInt();
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
		const auto layerId = _layersModel->data(_layersModel->rowCount() - 1 - row, layerColumnId(LayerColumn::ID), Qt::EditRole).toString();
		this->propByName<ImageLayerProp>(layerId)->render();
	}
}