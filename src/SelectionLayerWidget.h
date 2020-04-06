#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/** TODO */
class SelectionLayerWidget : public QWidget
{
public:
	/** TODO */
	SelectionLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*							_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::SelectionLayerWidget>	_ui;					/** TODO */
	LayersModel*								_layersModel;			/** TODO */
};