#pragma once

#include "Common.h"

#include <memory>

#include <QStackedWidget.h>

class ImageViewerPlugin;

namespace Ui {
	class SettingsWidget;
}

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

private:
	void onDatasetNamesChanged(const NameSet& datasetNames);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onImageNamesChanged(const NameSet& imageNames);
	void onCurrentImageChanged(const int& currentImage);
	void onDimensionNamesChanged(const NameSet& dimensionNames);
	void onCurrentDimensionChanged(const int& currentDimension);
	void onAverageImagesChanged(const bool& averageImages);

private:
	void update();

private:
	ImageViewerPlugin*					_imageViewerPlugin;
	std::unique_ptr<Ui::SettingsWidget>	_ui;
};
