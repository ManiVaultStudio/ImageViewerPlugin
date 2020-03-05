#pragma once

#include <memory>

#include <QWidget>

class DatasetsModel;

/** TODO */
class SettingsWidget : public QWidget
{
public: // Construction/destruction

	/** TODO */
	SettingsWidget(QWidget* parent, DatasetsModel* datasetsModel);

	/** Destructor */
	~SettingsWidget();
};