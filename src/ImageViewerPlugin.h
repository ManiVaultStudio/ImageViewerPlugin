#pragma once

#include "ViewPlugin.h"

#include "LayersModel.h"
#include "ImageViewerWidget.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class SettingsAction;
class NavigationAction;

namespace hdps {
    namespace gui {
        class DropWidget;
    }
}

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

    /** Returns a pointer to the core interface */
    hdps::CoreInterface* core() { return _core; }

public: // Miscellaneous

    /** Get the layers model */
    LayersModel& getModel() {
        return _model;
    }

    /** Get the layers selection model */
    QItemSelectionModel& getSelectionModel() {
        return _selectionModel;
    }

    ImageViewerWidget* getImageViewerWidget() {
        return _imageViewerWidget;
    }

public: // Action getters

    SettingsAction& getSettingsAction() { return *_settingsAction; }

private:
    LayersModel             _model;                 /** Layers model */
    QItemSelectionModel     _selectionModel;        /** Layers selection model */
    hdps::gui::DropWidget*  _dropWidget;            /** Widget for dropping data */
    QWidget*                _mainWidget;            /** Pointer to main widget */
    QSplitter*              _splitter;              /** Pointer to splitter */
    ImageViewerWidget*      _imageViewerWidget;     /** Pointer to image viewer widget */
    SettingsAction*         _settingsAction;        /** Pointer to settings action */
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