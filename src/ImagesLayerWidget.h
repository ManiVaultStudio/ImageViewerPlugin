#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ImagesLayerWidget;
}

/** TODO */
class ImagesLayerWidget : public QWidget
{
public:
	/** TODO */
	ImagesLayerWidget(QWidget* parent);

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
	std::unique_ptr<Ui::ImagesLayerWidget>	_ui;		/** TODO */
};