#include "ClustersDataset.h"

#include <QDebug>

ClustersDataset::ClustersDataset(ImageViewerPlugin* imageViewerPlugin, const QString& name) :
	Dataset(imageViewerPlugin, name, Type::Clusters)
{
}

void ClustersDataset::init()
{
}


/*
		const auto selectedRows = _datasetsModel->selectionModel()->selectedRows();

		if (selectedRows.size() == 1) {
			const auto row = selectedRows.first().row();
			const auto size = _datasetsModel->data(row, DatasetsModel::Columns::Size, Qt::EditRole).toSize();
			const auto clusters = _imageViewerPlugin->requestData<Clusters>(datasetName).getClusters();

			auto clustersImage = QImage(size.width(), size.height(), QImage::Format::Format_RGB32);

			const auto hueDelta = 360.0f / clusters.size();

			auto hue = 0.0f;

			for (auto& cluster : clusters) {
				for (auto id : cluster.indices) {
					const auto x = id % size.width();
					const auto y = floorf(static_cast<float>(id) / static_cast<float>(size.width()));
					clustersImage.setPixelColor(x, y, QColor::fromHsl(hue, 255, 100));
				}

				hue += hueDelta;
			}

			clustersImage.save("clustersImage.jpg");

			auto layersModel = _datasetsModel->layersModel(row);

			auto layer = new Layer(this, datasetName, datasetName, Layer::Type::MetaData, LayerFlags::Enabled, layersModel->rowCount());

			layer->setImage(clustersImage);

			_datasetsModel->layersModel(row)->add(layer);
		}
		*/

		//qDebug() << "No. clusters: " << clusters.getClusters().size();
		/*
		if (!hits.isEmpty()) {
			const auto row = hits.first().row();

			_datasetsModel->selectionModel()->select(_datasetsModel->index(row), QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::SelectionFlag::ClearAndSelect);
			_datasetsModel->selectionModel()->setCurrentIndex(_datasetsModel->index(row), QItemSelectionModel::SelectionFlag::Rows | QItemSelectionModel::SelectionFlag::ClearAndSelect);
		}
		*/