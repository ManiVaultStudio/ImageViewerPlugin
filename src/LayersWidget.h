#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayersWidget;
}

class QItemSelectionModel;

class ImageViewerPlugin;
class DatasetsModel;
class LayersModel;

/**
 * Layers widget class
 *
 * This widget class provides the user interface for editing layers
 *
 * @author Thomas Kroes
 */
class LayersWidget : public QWidget
{
public: // Construction/destruction

	/**
	 * Constructor
	 * @param parent Parent widget
	 */
	LayersWidget(QWidget* parent);

	/** Destructor */
	~LayersWidget() override;

public: // Initialization

	/**
	 * Initializes the widget
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	void initialize(ImageViewerPlugin* imageViewerPlugin);

private: // Drag and drop

	/**
	 * Invoked when an item is dragged into the widget
	 * @param dragEnterEvent Drag enter event
	 */
	void dragEnterEvent(QDragEnterEvent* dragEnterEvent) override;

	/**
	 * Invoked when an item is dropped onto the widget
	 * @param dropEvent Drop event
	 */
	void dropEvent(QDropEvent* dropEvent) override;

private: // Miscellaneous

	/** Returns the datasets model */
	DatasetsModel* getDatasetsModel();

	/** Returns the layers model */
	LayersModel& getLayersModel();

	/** Returns the layers selection model */
	QItemSelectionModel& getLayersSelectionModel();

private:
	ImageViewerPlugin*					_imageViewerPlugin;		/** Pointer to the image viewer plugin */
	std::unique_ptr<Ui::LayersWidget>	_ui;					/** User interface as produced by Qt designer */
};