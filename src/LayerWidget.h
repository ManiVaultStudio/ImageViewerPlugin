#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayerWidget;
}

class LayersModel;

/** TODO */
class LayerWidget : public QWidget
{
public:
	/** TODO */
	LayerWidget(QWidget* parent);

	/** TODO */
	void initialize(LayersModel* layersModel);

	/** TODO */
	void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles = QVector<int>());

private:
	std::unique_ptr<Ui::LayerWidget>	_ui;				/** TODO */
	LayersModel*						_layersModel;		/** TODO */
};