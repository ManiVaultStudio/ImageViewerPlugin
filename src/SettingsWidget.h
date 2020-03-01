#pragma once

#include <memory>

#include <QWidget>

class MainModel;

/** TODO */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SettingsWidget(QWidget* parent, MainModel* mainModel);

	/** Destructor */
	~SettingsWidget();
};