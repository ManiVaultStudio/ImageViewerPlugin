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
	QString imageCollectionType() const;
	bool isSequence() const;
	bool isStack() const;
	QStringList dimensionNames() const;
	QStringList imageFileNames() const;
	int noImages() const;
	QSize imageSize() const;
	
	QString currentDatasetName() const;
	void setCurrentDatasetName(const QString& currentDatasetName);
	int currentImageId() const;
	void setCurrentImageId(const int& currentImageId);
	int currentDimensionId() const;
	void setCurrentDimensionId(const int& currentDimensionId);
	bool averageImages() const;
	void setAverageImages(const bool& average);
	Indices displayImageIds() const;
	long noPixels() const;

private:
	void updateDisplayImageIds();
	
	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

signals:
	void currentDatasetNameChanged(const QString&);
	void currentImageIdChanged(const int&);
	void currentDimensionIdChanged(const int&);
	void averageImagesChanged(const bool& averageImages);
	void displayImageIdsChanged();
	void selectedPointsChanged();

private:
	ImageViewerWidget*		_imageViewerWidget;
	SettingsWidget*			_settingsWidget;
	QString					_currentDatasetName;
	int						_currentImageId;
	int						_currentDimensionId;
	bool					_averageImages;
	Indices					_displayImageIds;
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
