#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/** TODO */
class LayerWidget : public QWidget
{
public:
	/** TODO */
	LayerWidget(QWidget* parent);

	/** TODO */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*					_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::LayerWidget>	_ui;					/** TODO */
	LayersModel*						_layersModel;			/** TODO */
};