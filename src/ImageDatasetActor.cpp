#include "ImageDatasetActor.h"
#include "ImageLayerProp.h"
#include "ImageDataset.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QDebug>

ImageDatasetActor::ImageDatasetActor(Renderer* renderer, const QString& name, LayersModel* layersModel, const bool& visible /*= true*/) :
	Actor(renderer, name, visible),
	_layersModel(layersModel)
{
	for (int row = 0; row < _layersModel->rowCount(); row++) {
		const auto id	= _layersModel->data(row, LayersModel::Columns::ID, Qt::EditRole).toString();
		const auto type	= _layersModel->data(row, LayersModel::Columns::Type, Qt::EditRole).toInt();
		
		addProp<ImageLayerProp>(this, id, static_cast<Layer::Type>(type));
	}

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImageDatasetActor::updateData);
	

	QObject::connect(_layersModel, &LayersModel::rowsInserted, this, [this](const QModelIndex &parent, int first, int last) {
		for (int row = first; row <= last; row++) {
			const auto id	= _layersModel->data(row, LayersModel::Columns::ID, Qt::EditRole).toString();
			const auto type	= _layersModel->data(row, LayersModel::Columns::Type, Qt::EditRole).toInt();

			addProp<ImageLayerProp>(this, id, static_cast<Layer::Type>(type));
		}
	});

	updateData(_layersModel->index(0), _layersModel->index(_layersModel->rowCount() - 1, _layersModel->columnCount() - 1));
}

void ImageDatasetActor::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
	for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
		const auto layerId = _layersModel->data(row, LayersModel::Columns::ID, Qt::EditRole).toString();
		const auto layerProp = this->propByName<ImageLayerProp>(layerId);

		if (topLeft.column() <= LayersModel::Columns::Enabled && bottomRight.column() >= LayersModel::Columns::Enabled) {
			const auto enabled = _layersModel->data(row, LayersModel::Columns::Enabled, Qt::EditRole).toBool();
			layerProp->setVisible(enabled);
		}

		if (topLeft.column() <= LayersModel::Columns::Image && bottomRight.column() >= LayersModel::Columns::Image) {
			const auto layerImage = _layersModel->data(row, LayersModel::Columns::Image, Qt::EditRole).value<QImage>();
			layerProp->setImage(layerImage);
		}

		if (topLeft.column() <= LayersModel::Columns::DisplayRange && bottomRight.column() >= LayersModel::Columns::DisplayRange) {
			const auto layerDisplayRange = _layersModel->data(row, LayersModel::Columns::DisplayRange, Qt::EditRole).value<Layer::Range>();
			layerProp->setDisplayRange(layerDisplayRange.min(), layerDisplayRange.max());
		}

		if (topLeft.column() <= LayersModel::Columns::Opacity && bottomRight.column() >= LayersModel::Columns::Opacity) {
			const auto layerOpacity = _layersModel->data(row, LayersModel::Columns::Opacity, Qt::EditRole).toFloat();
			layerProp->setOpacity(layerOpacity);
		}

		if (topLeft.column() <= LayersModel::Columns::Order && bottomRight.column() >= LayersModel::Columns::Order) {
			const auto order = _layersModel->data(row, LayersModel::Columns::Order, Qt::EditRole).toInt();
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
		const auto layerId = _layersModel->data(_layersModel->rowCount() - 1 - row, LayersModel::Columns::ID, Qt::EditRole).toString();
		this->propByName<ImageLayerProp>(layerId)->render();
	}
}