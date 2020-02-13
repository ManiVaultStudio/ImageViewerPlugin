#pragma once

#include <memory>

#include <QWidget>

class Datasets;
class Dataset;

namespace Ui {
	class SettingsWidget;
}

/**
 * Settings widget class
 * @author Thomas Kroes
 */
class SettingsWidget : public QWidget
{
public:
	/**
	 * Constructor
	 * @param datasets Pointer to datasets
	 */
	SettingsWidget(Datasets* datasets);

	/** Destructor */
	~SettingsWidget();

private:
	/** 
	 * Invoked when data set names changed
	 * @param datasetNames Data set names
	 */
	void onDatasetNamesChanged(const QStringList& datasetNames);

	/** 
	 * Invoked when current data set changes
	 * @param currentDataset Current data set
	 */
	void onCurrentDatasetChanged(Dataset* previousDataset, Dataset* currentDataset);

private:
	Datasets*								_datasets;		/** Pointer to datasets */
	std::unique_ptr<Ui::SettingsWidget>		_ui;			/** UI */
};