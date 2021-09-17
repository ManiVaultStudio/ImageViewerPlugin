#pragma once

#include <ViewPlugin.h>

#include "Application.h"
#include "LayersModel.h"
#include "ColorMapModel.h"
#include "SettingsAction.h"

#include <QItemSelectionModel>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

class Images;
class ViewerWidget;
class StatusbarWidget;

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
    ImageViewerPlugin(const hdps::plugin::PluginFactory* factory);

public: // Inherited from ViewPlugin

    /** Initializes the plugin */
    void init() override;

    template<typename T>
    T& requestData(const QString& datasetName)
    {
        return _core->requestData<T>(datasetName);
    }

    /** Returns a pointer to the core interface */
    hdps::CoreInterface* core() { return _core; }

public: // Miscellaneous

    /** Returns the image viewer widget */
    ViewerWidget* getViewerWidget() {
        return _viewerWidget;
    }

    /** Returns the layer model */
    LayersModel& getLayersModel()
    {
        return _layersModel;
    }

    /** Returns the names of the points datasets in HDPS */
    QStringList getPointsDatasets() const {
        return _pointsDatasets;
    }

signals:

    /** Signals that list of point datasets in HDPS has changed */
    void pointsDatasetsChanged(QStringList pointsDatasets);

private:
    ViewerWidget*               _viewerWidget;          /** The image viewer widget */
    StatusbarWidget*            _statusbarWidget;       /** The status bar widget */
    LayersModel                 _layersModel;           /** Layers model */
    QStringList                 _pointsDatasets;        /** Point datasets loaded in HDPS */
    
    hdps::gui::DropWidget*      _dropWidget;            /** Widget for dropping data */
    SettingsAction              _settingsAction;        /** Settings action */
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