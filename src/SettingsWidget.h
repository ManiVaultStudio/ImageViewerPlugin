#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SettingsWidget;
}

class ImageDatasetsModel;
class ImageDataset;

/**
 * Settings widget class
 * @author Thomas Kroes
 */
class SettingsWidget : public QWidget
{
public:
	/** TODO */
	SettingsWidget(ImageDatasetsModel* imageDatasetsModel);

	/** Destructor */
	~SettingsWidget();

private:
	/** 
	 * Invoked when data set names changed
	 * @param datasetNames Data set names
	 */
	void onDatasetNamesChanged(const QStringList& datasetNames);

	/** TODO */
	void onCurrentDatasetChanged(ImageDataset* previousImageDataset, ImageDataset* currentImageDataset);

private:
	ImageDatasetsModel*						_imageDatasetsModel;	/** TODO */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/** UI */
};