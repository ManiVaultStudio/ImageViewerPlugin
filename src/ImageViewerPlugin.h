#pragma once

#include "ViewPlugin.h"

#include "LayersModel.h"
#include "SettingsAction.h"
#include "ImageViewerWidget.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class SettingsAction;

namespace hdps {
    namespace gui {
        class DropWidget;
    }
}

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

    /** Returns the layer model */
    LayersModel& getLayersModel() {
        return _layersModel;
    }

    ImageViewerWidget* getImageViewerWidget() {
        return _imageViewerWidget;
    }

private:
    LayersModel             _layersModel;           /** Layers model */
    hdps::gui::DropWidget*  _dropWidget;            /** Widget for dropping data */
    SettingsAction          _settingsAction;        /** Settings action */
    ImageViewerWidget*      _imageViewerWidget;     /** Pointer to image viewer widget */
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