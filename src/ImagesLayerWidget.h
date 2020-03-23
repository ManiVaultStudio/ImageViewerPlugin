#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ImagesLayerWidget;
}

class ImagesLayer;

/** TODO */
class ImagesLayerWidget : public QWidget
{
public:
	/** TODO */
	ImagesLayerWidget(QWidget* parent);

	/** TODO */
	void setImagesLayer(ImagesLayer* imagesLayer);

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
	std::unique_ptr<Ui::ImagesLayerWidget>	_ui;				/** TODO */
	ImagesLayer*							_imagesLayer;		/** TODO */
};