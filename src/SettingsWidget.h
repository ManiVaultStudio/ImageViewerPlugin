#pragma once

#include "Common.h"

#include <QWidget>

#include <memory>

class ImageViewerPlugin;

namespace Ui {
	class SettingsWidget;
}

class SettingsWidget : public QWidget
{
public:
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);
    ~SettingsWidget();
private:
	void onDatasetNamesChanged(const QStringList& datasetNames);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onImageNamesChanged(const QStringList& imageNames);
	void onCurrentImageChanged(const int& currentImage);
	void onDimensionNamesChanged(const QStringList& dimensionNames);
	void onCurrentDimensionChanged(const int& currentDimension);
	void onAverageImagesChanged(const bool& averageImages);
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);
	
	void updateSelectionOpacityUI();

private:
	ImageViewerPlugin*						_imageViewerPlugin;
	std::unique_ptr<Ui::SettingsWidget>		_ui;
};