#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionWidget;
}

class MainModel;

class QDataWidgetMapper;

/** TODO */
class SelectionWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SelectionWidget(QWidget* parent, MainModel* mainModel);

	/** Destructor */
	~SelectionWidget();

private:
	MainModel*								_mainModel;			/** TODO */
	std::unique_ptr<Ui::SelectionWidget>	_ui;				/** TODO */
};