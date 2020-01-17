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
	 * Invoked when data set names changed
	 * @param datasetNames Data set names
	 */
	void onDatasetNamesChanged(const QStringList& datasetNames);

	/** 
	 * Invoked when current data set changes
	 * @param currentDataset Current data set
	 */
	void onCurrentDatasetChanged(const QString& currentDataset);

	/** 
	 * Invoked when image names changes
	 * @param imageNames Image names
	 */
	void onImageNamesChanged(const QStringList& imageNames);

	/** 
	 * Invoked when the current image changes
	 * @param currentImage Current image index
	 */
	void onCurrentImageChanged(const int& currentImage);

	/** 
	 * Invoked when dimension names change
	  * @param dimensionNames Dimension names
	 */
	void onDimensionNamesChanged(const QStringList& dimensionNames);

	/** 
	 * Invoked when the current dimension changes
	 * @param currentDimension Current dimension index
	 */
	void onCurrentDimensionChanged(const int& currentDimension);

	/**
	 * Invoked when average images changes
	 * @param averageImages Whether sequence images should be averaged
	 */
	void onAverageImagesChanged(const bool& averageImages);

	/** 
	 * Invoked when the selection image changes
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