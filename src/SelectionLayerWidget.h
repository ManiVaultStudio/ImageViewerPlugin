#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionLayerWidget;
}

class LayersModel;

/** TODO */
class SelectionLayerWidget : public QWidget
{
public:
	/** TODO */
	SelectionLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(LayersModel* layersModel);

	/** TODO */
	void updateData(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	std::unique_ptr<Ui::SelectionLayerWidget>	_ui;				/** TODO */
	LayersModel*								_layersModel;		/** TODO */
};