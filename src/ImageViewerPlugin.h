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

	int noDimensions() const;

	ImageCollectionType imageCollectionType() const;
	QString currentImageFilePath() const;
	QString currentImageFileName() const;
	QString currentDimensionName() const;

	QStringList dimensionNames() const;
	QStringList imageFilePaths() const;

	std::size_t noImages() const;

	static std::size_t pixelId(const QSize& imageSize, const int& x, const int& y);
	static std::size_t pixelBufferOffset(const QSize& imageSize, const int& x, const int& y);
	static std::size_t sequenceCoordinateToPointId(const QSize& imageSize, const std::int32_t& imageId, const std::int32_t& noPixels, const std::int32_t& x, const std::int32_t& y);
	static std::size_t stackCoordinateToPointId(const QSize& imageSize, const std::int32_t& noDimensions, const std::int32_t& dimensionId, const std::int32_t& x, const std::int32_t& y);
	static std::size_t multipartSequenceCoordinateToPointId(const QSize& imageSize, const std::int32_t& noPointsPerDimension, const std::int32_t& imageOffset, const std::int32_t& currentDimensionId, const std::int32_t& x, const std::int32_t& y);

private:
	QSize imageSize() const;
	void update();
	void computeDisplayImage();

	void keyPressEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;
	void keyReleaseEvent(QKeyEvent* keyEvent) Q_DECL_OVERRIDE;

public:
	QString currentDataset() const;
	void setCurrentDataset(const QString& currentDataset);
	auto currentImageId() const;
	void setCurrentImageId(const std::int32_t& currentImageId);
	auto currentDimensionId() const;
	void setCurrentDimensionId(const std::int32_t& currentDimensionId);
	bool averageImages() const;
	void setAverageImages(const bool& averageImages);

private:
	void setDatasetNames(const QStringList& datasetNames);
	void setImageNames(const QStringList& imageNames);
	void setDimensionNames(const QStringList& dimensionNames);

signals:
	void datasetNamesChanged(const QStringList& datasetNames);
	void currentDatasetChanged(const QString& currentDataset);
	void imageNamesChanged(const QStringList& imageNames);
	void currentImageIdChanged(const int& currentImageId);
	void dimensionNamesChanged(const QStringList& dimensionNames);
	void currentDimensionIdChanged(const int& currentDimensionId);
	void averageImagesChanged(const bool& averageImages);
	void displayImageChanged(const QSize& imageSize, const TextureData& displayImage);
	void selectedPointsChanged();

private:
	ImageViewerWidget*	_imageViewerWidget;
	SettingsWidget*		_settingsWidget;
	QStringList			_datasetNames;
	QString				_currentDataset;
	QStringList			_imageNames;
	std::int32_t		_currentImageId;
	QStringList			_dimensionNames;
	std::int32_t		_currentDimensionId;
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
