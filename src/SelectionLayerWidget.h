#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionLayerWidget;
}

class ImageViewerPlugin;
class LayersModel;

/**
 * Selection layer widget class
 *
 * This widget class provides the user interface for editing selection layers
 *
 * @author Thomas Kroes
 */
class SelectionLayerWidget : public QWidget
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	SelectionLayerWidget(QWidget* parent);

public: // Initialization and update

	/**
	 * Initializes the widget
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

	/**
	 * Updates the UI with model indices ranging from \p begin to \p end
	 * @param begin Start of model index range
	 * @param end End of model index range
	 * @param roles Data roles
	 */
	void updateData(const QModelIndex& begin, const QModelIndex& end, const QVector<int>& roles = QVector<int>());

private:
	ImageViewerPlugin*							_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::SelectionLayerWidget>	_ui;					/** User interface as produced by Qt designer */
	LayersModel*								_layersModel;			/** Pointer to the layers model */
};