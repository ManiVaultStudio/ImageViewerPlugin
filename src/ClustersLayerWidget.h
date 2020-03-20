#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class ClustersLayerWidget;
}

class LayersModel;

/** TODO */
class ClustersLayerWidget : public QWidget
{
public:
	/** TODO */
	ClustersLayerWidget(QWidget* parent);

	/** TODO */
	void initialize(LayersModel* layersModel);

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
	std::unique_ptr<Ui::ClustersLayerWidget>	_ui;				/** TODO */
	LayersModel*								_layersModel;		/** TODO */
};