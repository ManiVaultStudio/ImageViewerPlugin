#include "ImageDatasetActor.h"
#include "ImageLayerProp.h"
#include "ImageDataset.h"
#include "LayersModel.h"
#include "Renderer.h"

#include <QOpenGLTexture>
#include <QDebug>

ImageDatasetActor::ImageDatasetActor(Renderer* renderer, const QString& name, LayersModel* layersModel, const bool& visible /*= true*/) :
	Actor(renderer, name, visible),
	_layersModel(layersModel)
{
	for (int row = 0; row < _layersModel->rowCount(); row++) {
		const auto layerName = _layersModel->data(row, LayersModel::Columns::Name).toString();
		addProp<ImageLayerProp>(this, layerName);
	}

	QObject::connect(_layersModel, &LayersModel::dataChanged, this, &ImageDatasetActor::updateData);

	updateData(_layersModel->index(0), _layersModel->index(_layersModel->rowCount() - 1, _layersModel->columnCount() - 1));
}

void ImageDatasetActor::updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
	for (int row = topLeft.row(); row <= bottomRight.row(); row++) {
		const auto layerName = _layersModel->data(row, LayersModel::Columns::Name).toString();
		const auto layerProp = this->propByName<ImageLayerProp>(layerName);

		if (topLeft.row() == 0 && topLeft.column() <= LayersModel::Columns::Image && bottomRight.column() >= LayersModel::Columns::Image) {
			const auto layerImage = _layersModel->data(row, LayersModel::Columns::Image, Qt::EditRole).value<QImage>();
			layerProp->setImage(layerImage);
		}

		if (topLeft.column() <= LayersModel::Columns::DisplayRange && bottomRight.column() >= LayersModel::Columns::DisplayRange) {
			const auto layerDisplayRange = _layersModel->data(row, LayersModel::Columns::DisplayRange, Qt::EditRole).value<LayerImage::Range>();
			layerProp->setDisplayRange(layerDisplayRange.min(), layerDisplayRange.max());
		}

		if (topLeft.column() <= LayersModel::Columns::Opacity && bottomRight.column() >= LayersModel::Columns::Opacity) {
			const auto layerOpacity = _layersModel->data(row, LayersModel::Columns::Opacity, Qt::EditRole).toFloat();
			layerProp->setOpacity(layerOpacity);
		}
	}
}