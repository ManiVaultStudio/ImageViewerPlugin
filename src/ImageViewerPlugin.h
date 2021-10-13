#pragma once

#include "ViewPlugin.h"

#include "LayersModel.h"
#include "ImageViewerWidget.h"

#include "widgets/DropWidget.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class SettingsAction;
class MainToolbarAction;
class NavigationAction;

class QSplitter;

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

    SettingsAction& getSettingsAction() { return *_settingsAction; }
    MainToolbarAction& getMainToolbarAction() { return *_mainToolbarAction; }
    NavigationAction& getNavigationAction() { return *_navigationAction; }

private:
    LayersModel             _model;                 /** Layers model */
    QItemSelectionModel     _selectionModel;        /** Layers selection model */
    QWidget*                _mainWidget;            /** Pointer to main widget */
    QSplitter*              _splitter;              /** Pointer to splitter */
    ImageViewerWidget       _imageViewerWidget;     /** Image viewer widget */
    DropWidget              _dropWidget;            /** Widget for dropping data */
    SettingsAction*         _settingsAction;        /** Pointer to settings action */
    MainToolbarAction*      _mainToolbarAction;     /** Pointer to main toolbar action */
    NavigationAction*       _navigationAction;      /** Pointer to navigation action */
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