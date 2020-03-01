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
	void setModel(LayersModel* previous, LayersModel* current);

private:
	DatasetsModel*						_datasetsModel;		/** TODO */
	std::unique_ptr<Ui::LayersWidget>	_ui;				/** TODO */
};