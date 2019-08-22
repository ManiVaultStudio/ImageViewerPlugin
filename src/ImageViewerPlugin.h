#pragma once

#include <ViewPlugin.h>

#include "PointsPlugin.h"

#include <QComboBox>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class ImageViewerWidget;
class SettingsWidget;

using Index = unsigned int;
using Indices = std::vector<Index>;

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
	int noImages() const;
	QSize imageSize() const;
	QString currentDataSetName() const;
	void setCurrentDataSetName(const QString& name);
	bool averageImages() const;
	void setAverageImages(const bool& averageImages);
	Index currentImageId() const;
	void setCurrentImageId(const int& currentImageId);
	Indices displayImageIds() const;

private:
	void updateDisplayImageIds();

signals:
	void currentDataSetNameChanged(const QString& currentDataSetName);
	void selectedPointsChanged();
	void averageImagesChanged(const bool& averageImages);
	void displayImageIdsChanged();

private:
	ImageViewerWidget*		_imageViewerWidget;
	SettingsWidget*			_settingsWidget;
	QString					_currentDataSetName;
	bool					_averageImages;
	int						_currentImageId;
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
