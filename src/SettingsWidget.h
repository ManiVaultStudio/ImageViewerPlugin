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
	void onDatasetNamesChanged(const QStringList& datasetNames);
	void onCurrentDatasetChanged(const QString& currentDataset);
	void onImageNamesChanged(const QStringList& imageNames);
	void onCurrentImageChanged(const int& currentImage);
	void onDimensionNamesChanged(const QStringList& dimensionNames);
	void onCurrentDimensionChanged(const int& currentDimension);
	void onAverageImagesChanged(const bool& averageImages);
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

private:
	ImageViewerPlugin*						_imageViewerPlugin;
	std::unique_ptr<Ui::SettingsWidget>		_ui;
};
