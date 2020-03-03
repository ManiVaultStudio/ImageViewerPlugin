#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayersWidget;
}

class DatasetsModel;
class LayersModel;

/** TODO */
class LayersWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	LayersWidget(QWidget* parent, DatasetsModel* datasetsModel);

	/** Destructor */
	~LayersWidget();

protected:

	/** TODO */
	void setModel(QSharedPointer<LayersModel> layersModel);

	/** TODO */
	void updateData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
	DatasetsModel*						_datasetsModel;		/** TODO */
	QSharedPointer<LayersModel>			_layersModel;		/** TODO */
	std::unique_ptr<Ui::LayersWidget>	_ui;				/** TODO */
};