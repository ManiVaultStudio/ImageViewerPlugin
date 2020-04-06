#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class PointsLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/** TODO */
class PointsLayerWidget : public QWidget
{
public:
	/** TODO */
	PointsLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::PointsLayerWidget>	_ui;					/** TODO */
	LayersModel*							_layersModel;			/** TODO */
};