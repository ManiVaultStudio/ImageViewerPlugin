#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SelectionWidget;
}

class DatasetsModel;

class QDataWidgetMapper;

/** TODO */
class SelectionWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SelectionWidget(QWidget* parent, DatasetsModel* datasetsModel);

	/** Destructor */
	~SelectionWidget();

private:
	DatasetsModel*							_datasetsModel;		/** TODO */
	std::unique_ptr<Ui::SelectionWidget>	_ui;				/** TODO */
};