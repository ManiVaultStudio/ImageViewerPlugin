#pragma once

#include <ViewPlugin.h>

#include "LayersModel.h"
#include "ColorMapModel.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class Images;
class ViewerWidget;
class StatusbarWidget;
class SettingsWidget;

/**
 * Image viewer plugin class
 * This HDPS view plugin class provides functionality to view/interact with high-dimensional image data
 * 
 * @author Thomas Kroes
 */
class ImageViewerPlugin : public ViewPlugin
{
	Q_OBJECT

public:
	/** Constructor */
	ImageViewerPlugin();

public: // Inherited from ViewPlugin

	/** Initializes the plugin */
	void init() override;

	/** Invoked when image data is added */
	void dataAdded(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when image data changes */
	void dataChanged(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when image data is removed */
	void dataRemoved(const QString name) Q_DECL_OVERRIDE;

	/** Invoked when the selection changes */
	void selectionChanged(const QString dataset) Q_DECL_OVERRIDE;

	/** Determines which data types this the image viewer is compatible with */
	hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

	template<typename T>
	T& requestData(const QString& datasetName)
	{
		return _core->requestData<T>(datasetName);
	}

	/** Returns a pointer to the core interface */
	hdps::CoreInterface* core() { return _core; }

	/**
	 * Event filter
	 *@param target Target object
	 *@param event Event that occurred
	 */
	bool eventFilter(QObject* target, QEvent* event) override;

public: // Miscellaneous

	/** Returns the image viewer widget */
	ViewerWidget* getViewerWidget() {
		return _viewerWidget;
	}

	/** Returns the settings widget */
	SettingsWidget* getSettingsWidget()
	{
		return _settingsWidget;
	}

	/** Returns the layer model */
	LayersModel& getLayersModel()
	{
		return _layersModel;
	}

	/** Returns the color map model */
	ColorMapModel& getColorMapModel()
	{
		return _colorMapModel;
	}

	/** Returns the names of the points datasets in HDPS */
	QStringList getPointsDatasets() const {
		return _pointsDatasets;
	}

signals:
	
	/** Signals that list of point datasets in HDPS has changed */
	void pointsDatasetsChanged(QStringList pointsDatasets);

	/** Signals that the selection of a dataset has changed */
	void selectionIndicesChanged(const QString& name);

private:
	ViewerWidget*		_viewerWidget;			/** The image viewer widget */
	StatusbarWidget*	_statusbarWidget;		/** The status bar widget */
	SettingsWidget*		_settingsWidget;		/** Settings widget */
	LayersModel			_layersModel;			/** Layers model */
	ColorMapModel		_colorMapModel;			/** Colormap model */
	QStringList			_pointsDatasets;		/** Point datasets loaded in HDPS */
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
	/** Default constructor */
	ImageViewerPluginFactory() {}

	/** Destructor */
	~ImageViewerPluginFactory() override {}

	/** Creates an image viewer plugin instance */
	ImageViewerPlugin* produce() override;
};