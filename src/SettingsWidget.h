#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SettingsWidget;
}

class MainModel;

class QDataWidgetMapper;

/**
 * Settings widget class
 * @author Thomas Kroes
 */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SettingsWidget(MainModel* mainModel);

	/** Destructor */
	~SettingsWidget();

private:
	MainModel*								_mainModel;				/** TODO */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/** UI */
	QDataWidgetMapper*						_dataWidgetMapper;		/** TODO */
};