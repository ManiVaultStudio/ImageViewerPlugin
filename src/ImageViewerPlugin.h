#pragma once

#include <ViewPlugin.h>

#include "PointsPlugin.h"
#include "Common.h"

#include <QComboBox>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class ImageViewerWidget;
class SettingsWidget;

class ImageViewerPlugin : public ViewPlugin
{
	Q_OBJECT

public:
	ImageViewerPlugin();
    ~ImageViewerPlugin(void);

	void init() override;

	void dataAdded(const QString name) Q_DECL_OVERRIDE;
	void dataChanged(const QString name) Q_DECL_OVERRIDE;
	void dataRemoved(const QString name) Q_DECL_OVERRIDE;
	void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
	QStringList supportedDataKinds() Q_DECL_OVERRIDE;

public:
	PointsPlugin& pointsData() const;
	Indices selection() const;
	void setSelection(Indices& indices);
	bool hasSelection() const;

	ImageCollectionType imageCollectionType() const;
	QString currentImageName() const;

	QStringList dimensionNames() const;
	QStringList imageFilePaths() const;

	int noImages() const;
	QSize imageSize() const;
	
	long noPixels() const;

private:
	void update();
	void computeDisplayImage();

	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

public:
	QString currentDataset() const;
	void setCurrentDataset(const QString& currentDataset);
	auto currentImage() const;
	void setCurrentImage(const int& currentImage);
	auto currentDimension() const;
	void setCurrentDimension(const int& currentDimension);
	bool averageImages() const;
	void setAverageImages(const bool& averageImages);
	Indices displayImages() const;

private:
	void setDatasetNames(const QStringList& datasetNames);
	void setImageNames(const QStringList& imageNames);
	void setDimensionNames(const QStringList& dimensionNames);

signals:
	void datasetNamesChanged(const QStringList& datasetNames);
	void currentDatasetChanged(const QString& currentDataset);
	void imageNamesChanged(const QStringList& imageNames);
	void currentImageChanged(const int& currentImage);
	void dimensionNamesChanged(const QStringList& dimensionNames);
	void currentDimensionChanged(const int& currentDimension);
	void averageImagesChanged(const bool& averageImages);
	void displayImageChanged(const TextureData& displayImage);

private:
	ImageViewerWidget*	_imageViewerWidget;
	SettingsWidget*		_settingsWidget;
	QStringList			_datasetNames;
	QString				_currentDataset;
	QStringList			_imageNames;
	int					_currentImage;
	QStringList			_dimensionNames;
	int					_currentDimension;
	bool				_averageImages;
};

class ImageViewerPluginFactory : public ViewPluginFactory
{
	Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ImageViewerPlugin"
                      FILE  "ImageViewerPlugin.json")
    
public:
	ImageViewerPluginFactory(void) {}
    ~ImageViewerPluginFactory(void) {}
    
	ImageViewerPlugin* produce();
};
