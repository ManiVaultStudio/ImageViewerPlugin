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

protected:

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private: // Drag and drop

	/** TODO */
	void dragEnterEvent(QDragEnterEvent* dragEnterEvent);

	/** TODO */
	void dropEvent(QDropEvent* dropEvent);

private: // Miscellaneous

	/** TODO */
	LayersModel* layersModel();

private:
	ImageViewerPlugin*					_imageViewerPlugin;		/** TODO */
	std::unique_ptr<Ui::LayersWidget>	_ui;					/** TODO */
};