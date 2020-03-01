#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class LayersWidget;
}

class MainModel;

class QDataWidgetMapper;

/** TODO */
class LayersWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	LayersWidget(MainModel* mainModel);

	/** Destructor */
	~LayersWidget();

private:
	MainModel*							_mainModel;				/** TODO */
	std::unique_ptr<Ui::LayersWidget>	_ui;					/** TODO */
};