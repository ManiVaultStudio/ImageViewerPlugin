#pragma once

#include <ViewPlugin.h>

#include "DatasetsModel.h"
#include "LayersModel.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class ViewerWidget;
class SettingsWidget;

/**
 * Image viewer plugin class
 * This HDPS view plugin class provides functionality to view/interact with high-dimensional image data
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
	void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;

	/** Determines which data types this the image viewer is compatible with */
	hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

	/** TODO */
	template<typename T>
	T& requestData(const QString& datasetName)
	{
		return _core->requestData<T>(datasetName);
	}

public: //

	/** TODO */
	DatasetsModel& datasetsModel() { return _datasetsModel; }

	/** TODO */
	QItemSelectionModel& datasetsSelectionModel() { return _datasetsSelectionModel; }

	/** TODO */
	LayersModel& layersModel() { return _layersModel; }

	/** Update the window title with some useful meta data */
	void updateWindowTitle();

private:
	ViewerWidget*			_imageViewerWidget;			/** TODO */
	SettingsWidget*			_settingsWidget;			/** TODO */
	DatasetsModel			_datasetsModel;				/** TODO */
	QItemSelectionModel		_datasetsSelectionModel;	/** TODO */
	LayersModel				_layersModel;				/** TODO */
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
	~ImageViewerPluginFactory() {}

	/** Creates an image viewer plugin instance */
	ImageViewerPlugin* produce();
};