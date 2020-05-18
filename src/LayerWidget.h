#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/**
 * Layer widget class
 *
 * This widget class provides the user interface for editing layer common properties
 *
 * @author Thomas Kroes
 */
class LayerWidget : public QWidget
{
public: // Construction

	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	LayerWidget(QWidget* parent);

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
	ImageViewerPlugin*					_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::LayerWidget>	_ui;					/** User interface as produced by Qt designer */
	LayersModel*						_layersModel;			/** Pointer to the layers model */
};