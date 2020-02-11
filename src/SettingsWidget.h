#pragma once

#include "Datasets.h"

#include <memory>

#include <QWidget>

class ImageViewerPlugin;

namespace Ui {
	class SettingsWidget;
}

/**
 * Settings widget class
 * This widget provides a user interface to interact with high-dimensional image data
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

	/** 
	 * Invoked when the selection image changes
	 * @param selectionImage Selection image
	 * @param selectionBounds Two-dimensional boundaries of the selected pixel in the selection image
	 */
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);
	
	/** Update the selection opacity user interface */
	void updateSelectionOpacityUI();

private:
	Datasets*								_datasets;		/** Pointer to datasets */
	std::unique_ptr<Ui::SettingsWidget>		_ui;			/** UI */
};