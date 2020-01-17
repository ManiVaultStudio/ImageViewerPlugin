#pragma once

#include <ViewPlugin.h>

#include "ImageData/Images.h"

#include "Common.h"

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class ImageViewerWidget;
class SettingsWidget;

/**
 * Image viewer plugin class
 * This HDPS view plugin class provides functionality to view/interact with high-dimensional image data
 */
class ImageViewerPlugin : public ViewPlugin
{
	Q_OBJECT

public:
	/** Constructor */
	ImageViewerPlugin();

	/** Initializes the plugin */
	void init() override;

	/** Invoked when image data is added */
	void dataAdded(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when image data changes */
	void dataChanged(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when image data is removed */
	void dataRemoved(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when the selection changes */
	void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;

	/** Determines which data types this the image viewer is compatible with */
	hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

public:
	/** Returns the selection */
	std::vector<std::uint32_t> selection() const;

	/** Returns whether a selection is present */
	bool hasSelection() const;

	/** Returns the the type of image collection e.g. sequence, stack*/
	ImageCollectionType imageCollectionType() const;

	/** Whether image data set can be selected */
	bool allowsPixelSelection() const;

	/** Current image size */
	QSize imageSize() const;

	/**
	 * Select pixels in the high-dimensional image data
	 * @param pixelCoordinates Pixel coordinates to select
	 * @param selectionModifier Selection modifier
	 */
	void selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier);

	/** Returns the number of selected pixels */
	std::uint32_t noSelectedPixels();

public:
	/** Update the internals */
	void update();

	/** Compute the display image */
	void computeDisplayImage();

	/** Compute the selection image */
	void computeSelectionImage();

public:
	/** Returns a list of available image data sets */
	QStringList datasetNames() const;

	/** Returns the current images */
	Images* currentImages();

	/** Returns the current dataset name */
	QString currentDatasetName() const;

	/**
	 * Sets the current dataset name
	 * @param currentDatasetName Current dataset name
	 */
	void setCurrentDatasetName(const QString& currentDatasetName);

	/** Returns the current image index */
	auto currentImageId() const;

	/**
	 * Sets the current image index
	 * @param currentImageId Current image index
	 */
	void setCurrentImageId(const std::int32_t& currentImageId);

	/** Returns the current dimension index */
	auto currentDimensionId() const;

	/**
	 * Sets the current dimension index
	 * @param currentDimensionId Current dimension index
	 */
	void setCurrentDimensionId(const std::int32_t& currentDimensionId);

	/** Returns whether sequence images are averaged */
	bool averageImages() const;

	/**
	 * Sets whether sequence images are averaged
	 * @param averageImages Whether sequence images are averaged
	 */
	void setAverageImages(const bool& averageImages);

	/** Creates an image subset from the current selection */
	void createSubsetFromSelection();

	/** Returns the image viewer widget */
	ImageViewerWidget* imageViewerWidget();

private:
	/**
	 * Set dataset names
	 * @param datasetNames Data set names
	 */
	void setDatasetNames(const QStringList& datasetNames);

	/**
	 * Set image names
	 * @param imageNames Image names
	 */
	void setImageNames(const QStringList& imageNames);

	/**
	 * Set dimension names
	 * @param dimensionNames Dimension names
	 */
	void setDimensionNames(const QStringList& dimensionNames);

	/** Update the window title with some useful meta data */
	void updateWindowTitle();

signals:
	/**
	 * Invoked when the dataset names change
	 * @param datasetNames Dataset names
	 */
	void datasetNamesChanged(const QStringList& datasetNames);

	/**
	 * Invoked when the current dataset changes
	 * @param currentDataset Current dataset name
	 */
	void currentDatasetChanged(const QString& currentDataset);

	/**
	 * Invoked when the image names change
	 * @param imageNames Image names
	 */
	void imageNamesChanged(const QStringList& imageNames);

	/**
	 * Invoked when the current image index changes
	 * @param currentImageId Current image index
	 */
	void currentImageIdChanged(const std::int32_t& currentImageId);

	/**
	 * Invoked when the dimension names change
	 * @param dimensionNames Dimension names
	 */
	void dimensionNamesChanged(const QStringList& dimensionNames);

	/**
	 * Invoked when the current dimension index changes
	 * @param currentDimensionId Current dimension index
	 */
	void currentDimensionIdChanged(const std::int32_t& currentDimensionId);

	/**
	 * Invoked when average images changes
	 * @param averageImages Average images
	 */
	void averageImagesChanged(const bool& averageImages);

	/**
	 * Invoked when the display image changes
	 * @param displayImage Display image
	 */
	void displayImageChanged(std::shared_ptr<QImage> displayImage);

	/**
	 * Invoked when the selection image changes
	 * @param selectionImage Selection image
	 * @param selectionBounds Bounds of the selected pixels in the selection image
	 */
	void selectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

private:
	ImageViewerWidget*	_imageViewerWidget;			/*! Pointer to image viewer widget */
	SettingsWidget*		_settingsWidget;			/*! Image viewer settings widget */
	QStringList			_datasetNames;				/*! Available datasets */
	QString				_currentDatasetName;		/*! Name of the currently displayed image dataset */
	Images*				_currentImages;				/*! Pointer to the Images HDPS data structure */
	QStringList			_imageNames;				/*! Image names */
	std::int32_t		_currentImageId;			/*! The index of the current image  */
	QStringList			_dimensionNames;			/*! Dimension names */
	std::int32_t		_currentDimensionId;		/*! The index of the current dimension */
	bool				_averageImages;				/*! Whether to average images (ImageCollectionType::Sequence) */
};

/**
 * Image viewer plugin factory class
 * A factory for creating image viewer plugin instances
 */
class ImageViewerPluginFactory : public ViewPluginFactory
{
	Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin" FILE "ImageViewerPlugin.json")

public:
	/** Constructor */
	ImageViewerPluginFactory() {}

	/** Destructor */
	~ImageViewerPluginFactory() {}

	/** Create an image viewer plugin instance */
	ImageViewerPlugin* produce();
};