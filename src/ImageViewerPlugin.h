#pragma once

#include <ViewPlugin.h>

#include "ImageData/ImageDataSet.h"

#include "Common.h"

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class ImageViewerWidget;
class SettingsWidget;

class ImageViewerPlugin : public ViewPlugin
{
	Q_OBJECT

public:
	ImageViewerPlugin();

	void init() override;

	void dataAdded(const QString name) Q_DECL_OVERRIDE;
	void dataChanged(const QString name) Q_DECL_OVERRIDE;
	void dataRemoved(const QString name) Q_DECL_OVERRIDE;
	void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
	QStringList supportedDataKinds() Q_DECL_OVERRIDE;

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
	ImageDataSet* currentImageDataSet();
	QString currentDatasetName() const;
	void setCurrentDatasetName(const QString& currentDatasetName);
	auto currentImageId() const;
	void setCurrentImageId(const std::int32_t& currentImageId);
	auto currentDimensionId() const;
	void setCurrentDimensionId(const std::int32_t& currentDimensionId);
	bool averageImages() const;
	void setAverageImages(const bool& averageImages);
	float selectionOpacity() const;
	void setSelectionOpacity(const float& selectionOpacity);
	void createSubsetFromSelection();

private:
	void setDatasetNames(const QStringList& datasetNames);
	void setImageNames(const QStringList& imageNames);
	void setDimensionNames(const QStringList& dimensionNames);

signals:
	void datasetNamesChanged(const QStringList& datasetNames);
	void currentDatasetChanged(const QString& currentDataset);
	void imageNamesChanged(const QStringList& imageNames);
	void currentImageIdChanged(const std::int32_t& currentImageId);
	void dimensionNamesChanged(const QStringList& dimensionNames);
	void currentDimensionIdChanged(const std::int32_t& currentDimensionId);
	void averageImagesChanged(const bool& averageImages);
	void selectionOpacityChanged(const float& selectionOpacity);
	void displayImageChanged(std::shared_ptr<QImage> displayImage);
	void selectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds);

private:
	ImageViewerWidget*	_imageViewerWidget;
	SettingsWidget*		_settingsWidget;
	QStringList			_datasetNames;
	QString				_currentDatasetName;
	ImageDataSet*		_currentImageDataSet;
	QStringList			_imageNames;
	std::int32_t		_currentImageId;
	QStringList			_dimensionNames;
	std::int32_t		_currentDimensionId;
	bool				_averageImages;
	float				_selectionOpacity;
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
