#pragma once

#include "Common.h"

#include <memory>

#include <QWidget>

class ImageViewerPlugin;

namespace Ui {
	class SettingsWidget;
}

/**
 * Settings widget class
 * This widget provides a user interface to interact with high-dimensional image data
 */
class SettingsWidget : public QWidget
{
public:
	/**
	 * Constructor
	 * @param imageViewerPlugin Pointer to the image viewer plugin
	 */
	SettingsWidget(ImageViewerPlugin* imageViewerPlugin);

	/** Destructor */
	~SettingsWidget();

private:
	/** 
	 * Triggered when data set names changed
	 * @param datasetNames Data set names
	 */
	void onDatasetNamesChanged(const QStringList& datasetNames);

	/** 
	 * Triggered when the current data set changed
	 * @param currentDataset Current data set
	 */
	void onCurrentDatasetChanged(const QString& currentDataset);

	/** 
	 * Triggered when image names changed
	 * @param imageNames Image names
	 */
	void onImageNamesChanged(const QStringList& imageNames);

	/** 
	 * Triggered when the current image changed
	 * @param currentImage Current image index
	 */
	void onCurrentImageChanged(const int& currentImage);

	/** 
	 * Triggered when dimension names changed
	  * @param dimensionNames Dimension names
	 */
	void onDimensionNamesChanged(const QStringList& dimensionNames);

	/** 
	 * Triggered when current dimension changed
	 * @param currentDimension Current dimension index
	 */
	void onCurrentDimensionChanged(const int& currentDimension);

	/**
	 * Triggered when average images changed
	 * @param averageImages Whether sequence images should be averaged
	 */
	void onAverageImagesChanged(const bool& averageImages);

	/** 
	 * Triggered when the selection image changed
	 * @param selectionImage Selection image
	 * @param selectionBounds Two-dimensional boundaries of the selected pixel in the selection image
	 */
	void onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);
	
	/** Update the selection opacity user interface */
	void updateSelectionOpacityUI();

private:
	ImageViewerPlugin*						_imageViewerPlugin;		/*! Pointer to the image viewer plugin */
	std::unique_ptr<Ui::SettingsWidget>		_ui;					/*! UI */
};