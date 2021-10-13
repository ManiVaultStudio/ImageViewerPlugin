#pragma once

#include "ViewPlugin.h"

#include "LayersModel.h"
#include "ImageViewerWidget.h"
#include "MainToolbarAction.h"
#include "ZoomToolbarAction.h"
#include "SettingsAction.h"

#include "widgets/DropWidget.h"

#include <QItemSelectionModel>
#include <QSplitter>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

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
    ImageViewerPlugin(hdps::plugin::PluginFactory* factory);

public: // Inherited from ViewPlugin

    /** Initializes the plugin */
    void init() override;

public: // Miscellaneous

    /** Get the layers model */
    LayersModel& getModel() {
        return _model;
    }

    /** Get the layers selection model */
    QItemSelectionModel& getSelectionModel() {
        return _selectionModel;
    }

    /** Get reference to the image viewer widget */
    ImageViewerWidget& getImageViewerWidget() {
        return _imageViewerWidget;
    }

protected:

    /** Update the window title */
    void updateWindowTitle();

public: // Action getters

    MainToolbarAction& getMainToolbarAction() { return _mainToolbarAction; }
    ZoomToolbarAction& getZoomToolbarAction() { return _zoomToolbarAction; }
    SettingsAction& getSettingsAction() { return _settingsAction; }

private:
    LayersModel             _model;                 /** Layers model */
    QItemSelectionModel     _selectionModel;        /** Layers selection model */
    QSplitter               _splitter;              /** Splitter which divides the layers view and editor */
    ImageViewerWidget       _imageViewerWidget;     /** Image viewer widget */
    DropWidget              _dropWidget;            /** Widget for dropping data */
    MainToolbarAction       _mainToolbarAction;     /** Main toolbar action */
    ZoomToolbarAction       _zoomToolbarAction;     /** Zoom toolbar action */
    SettingsAction          _settingsAction;        /** Layers settings action */
};

/**
 * Image viewer plugin factory class
 * A factory for creating image viewer plugin instances
 */
class ImageViewerPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin" FILE "ImageViewerPlugin.json")

public:
    /** Default constructor */
    ImageViewerPluginFactory() {}

    /** Destructor */
    ~ImageViewerPluginFactory() override {}

    /** Returns the plugin icon */
    QIcon getIcon() const override;

    /** Creates an image viewer plugin instance */
    ImageViewerPlugin* produce() override;

    hdps::DataTypes supportedDataTypes() const override;
};