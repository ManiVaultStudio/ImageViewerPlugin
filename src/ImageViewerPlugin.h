#pragma once

#include <ViewPlugin.h>

#include "LayersModel.h"
#include "ColorMapModel.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class Images;
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
	void selectionChanged(const QString dataset) Q_DECL_OVERRIDE;

	/** Determines which data types this the image viewer is compatible with */
	hdps::DataTypes supportedDataTypes() const Q_DECL_OVERRIDE;

	/** TODO */
	template<typename T>
	T& requestData(const QString& datasetName)
	{
		return _core->requestData<T>(datasetName);
	}

	/** Returns a pointer to the core interface */
	hdps::CoreInterface* core() { return _core; }

private: // Mouse/keyboard events

	/**
	 * Invoked when a key is pressed
	 * @param keyEvent Key event
	 */
	void keyPressEvent(QKeyEvent* keyEvent) override;

	/**
	 * Invoked when a key is released
	 * @param keyEvent Key event
	 */
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

	/**
	 * Invoked when the mouse button is pressed
	 * @param mouseEvent Mouse event
	 */
	void mousePressEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse pointer is moved
	 * @param mouseEvent Mouse event
	 */
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse button is released
	 * @param mouseEvent Mouse event
	 */
	void mouseReleaseEvent(QMouseEvent* mouseEvent) override;

	/**
	 * Invoked when the mouse wheel is rotated
	 * @param wheelEvent Mouse wheel event
	 */
	void wheelEvent(QWheelEvent* wheelEvent) override;

public: //

	/** Returns the image viewer widget */
	ViewerWidget* viewerWidget() { return _viewerWidget; }

	/** Returns the settings widget */
	SettingsWidget* settingsWidget() { return _settingsWidget; }

	/** TODO */
	LayersModel& layersModel() { return _layersModel; }

	/** TODO */
	ColorMapModel& colorMapModel() { return _colorMapModel; }

	QVector<QString> imageDatasets() const { return _imagesDatasets; }

	/** TODO */
	Images* sourceImagesSetFromPointsSet(const QString& pointSetName);

private:
	ViewerWidget*			_viewerWidget;			/** TODO */
	SettingsWidget*			_settingsWidget;		/** TODO */
	LayersModel				_layersModel;			/** TODO */
	ColorMapModel			_colorMapModel;			/** TODO */
	QVector<QString>		_imagesDatasets;		/** Very unhappy with this approach, but there's (currently) no other way */
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