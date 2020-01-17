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

	/** Invoked when image data is changed */
	void dataChanged(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when image data is removed */
	void dataRemoved(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when selection has changed */
	void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;

	/** Determines which data types this the image viewer is compatible with */
	hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

public:
	std::vector<std::uint32_t> selection() const;
	bool hasSelection() const;
	ImageCollectionType imageCollectionType() const;
	bool selectable() const;
	QSize imageSize() const;
	void selectPixels(const std::vector<std::pair<std::uint32_t, std::uint32_t>>& pixelCoordinates, const SelectionModifier& selectionModifier);
	std::uint32_t noSelectedPixels();

public:
	void update();
	void computeDisplayImage();
	void computeSelectionImage();

public:
	QStringList datasetNames() const;
	Images* currentImages();
	QString currentDatasetName() const;
	void setCurrentDatasetName(const QString& currentDatasetName);
	auto currentImageId() const;
	void setCurrentImageId(const std::int32_t& currentImageId);
	auto currentDimensionId() const;
	void setCurrentDimensionId(const std::int32_t& currentDimensionId);
	bool averageImages() const;
	void setAverageImages(const bool& averageImages);
	void createSubsetFromSelection();
	ImageViewerWidget* imageViewerWidget();

private:
	void setDatasetNames(const QStringList& datasetNames);
	void setImageNames(const QStringList& imageNames);
	void setDimensionNames(const QStringList& dimensionNames);
	void updateWindowTitle();

signals:
	void datasetNamesChanged(const QStringList& datasetNames);
	void currentDatasetChanged(const QString& currentDataset);
	void imageNamesChanged(const QStringList& imageNames);
	void currentImageIdChanged(const std::int32_t& currentImageId);
	void dimensionNamesChanged(const QStringList& dimensionNames);
	void currentDimensionIdChanged(const std::int32_t& currentDimensionId);
	void averageImagesChanged(const bool& averageImages);
	void displayImageChanged(std::shared_ptr<QImage> displayImage);
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

class ImageViewerPluginFactory : public ViewPluginFactory
{
	Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ImageViewerPlugin"
                      FILE  "ImageViewerPlugin.json")
    
public:
	ImageViewerPluginFactory() {}
    ~ImageViewerPluginFactory() {}
    
	ImageViewerPlugin* produce();
};