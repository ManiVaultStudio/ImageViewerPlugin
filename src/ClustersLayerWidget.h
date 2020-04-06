#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ClustersLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/** TODO */
class ClustersLayerWidget : public QWidget
{
public:
	/** TODO */
	ClustersLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles = QVector<int>());

private:
	ImageViewerPlugin*							_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::ClustersLayerWidget>	_ui;					/** TODO */
	LayersModel*								_layersModel;			/** TODO */
};