#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionWidget;
}

class ImageViewerPlugin;
class DatasetsModel;

class QDataWidgetMapper;

/** TODO */
class SelectionWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SelectionWidget(ImageViewerPlugin* imageViewerPlugin, DatasetsModel* datasetsModel);

	/** Destructor */
	~SelectionWidget();

protected:

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/** TODO */
	DatasetsModel*							_datasetsModel;			/** TODO */
	std::unique_ptr<Ui::SelectionWidget>	_ui;					/** TODO */
};