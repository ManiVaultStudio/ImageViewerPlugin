#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ImagesLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/**
 * Images layer widget class
 *
 * This widget class provides the user interface for editing images layers
 *
 * @author Thomas Kroes
 */
class ImagesLayerWidget : public QWidget
{
public:  // Construction

	/** TODO */
	ImagesLayerWidget(QWidget* parent);

public: // Initialization and update

	/**
	 * Initializes the widget
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/**
	 * Updates the user interface with data from the given layer model indices
	 * @param topLeft Top left model index
	 * @param bottomRight Bottom right model index
	 * @param roles Data roles
	 */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::ImagesLayerWidget>	_ui;					/** User interface as produced by Qt designer */
	LayersModel*							_layersModel;			/** Pointer to the layers model */
};