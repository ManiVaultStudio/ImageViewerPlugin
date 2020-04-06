#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ImagesLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/** TODO */
class ImagesLayerWidget : public QWidget
{
public:
	/** TODO */
	ImagesLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::ImagesLayerWidget>	_ui;					/** TODO */
	LayersModel*							_layersModel;			/** TODO */
};