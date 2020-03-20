#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayersWidget;
}

class ImageViewerPlugin;
class DatasetsModel;
class LayersModel;

/** TODO */
class LayersWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	LayersWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~LayersWidget();

private: // Drag and drop

	/** TODO */
	void dragEnterEvent(QDragEnterEvent* dragEnterEvent);

	/** TODO */
	void dropEvent(QDropEvent* dropEvent);

private: // Miscellaneous

	/** TODO */
	DatasetsModel* datasetsModel();

	/** TODO */
	LayersModel* layersModel();

private:
	ImageViewerPlugin*					_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::LayersWidget>	_ui;					/** TODO */
};