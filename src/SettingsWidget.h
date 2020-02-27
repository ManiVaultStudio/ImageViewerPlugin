#pragma once

#include <memory>

#include <QWidget>

namespace Ui {
	class SettingsWidget;
}

class ImageDatasetsModel;
class ImageDataset;

class QDataWidgetMapper;

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
	/** TODO */
	void onCurrentDatasetChanged(ImageDataset* previousImageDataset, ImageDataset* currentImageDataset);

private:
	ImageDatasetsModel*						_imageDatasetsModel;	/** TODO */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/** UI */
	QDataWidgetMapper*						_dataWidgetMapper;		/** TODO */
};