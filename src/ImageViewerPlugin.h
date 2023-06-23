#pragma once

#include "ViewPlugin.h"

#include "LayersModel.h"
#include "ImageViewerWidget.h"
#include "SelectionToolbarAction.h"
#include "InteractionToolbarAction.h"
#include "SettingsAction.h"

#include <widgets/DropWidget.h>
#include <actions/TriggerAction.h>

#include <QItemSelectionModel>
#include <QSplitter>

using hdps::plugin::ViewPluginFactory;
using hdps::plugin::ViewPlugin;

using namespace hdps::gui;

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

    /** Spatial layout of layers */
    enum class LayersLayout {
        Stacked,            /** Layers are stacked on top of each other */
        Vertical,           /** Layers are arranged from top to bottom */
        Horizontal,         /** Layers are arranged from left to right */
        Grid                /** Layers are arranged in a grid */
    };

public:
    /** Constructor */
    ImageViewerPlugin(hdps::plugin::PluginFactory* factory);

public: // Inherited from ViewPlugin

    /** Initializes the plugin */
    void init() override;

    /**
     * Load one (or more datasets in the view)
     * @param datasets Dataset(s) to load
     */
    void loadData(const hdps::Datasets& datasets) override;

    /**
     * Arrange layers in layout
     * @param layersLayout Layout of the layers
     */
    void arrangeLayers(LayersLayout layersLayout);

    /**
     * Add dataset to the viewer
     * @param dataset Smart pointer to images dataset
     */
    void addDataset(const hdps::Dataset<Images>& dataset);

public: // Miscellaneous

    /** Get the layers model */
    LayersModel& getLayersModel() {
        return _layersModel;
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

    /** Invoked when the layer selection changed */
    void onLayerSelectionChanged();

    /**
     * Converts a non-images dataset to an images dataset and adds the created dataset as a layer
     * @param dataset Smart pointer to the dataset that will be converted and added
     */
    void immigrateDataset(const hdps::Dataset<hdps::DatasetImpl>& dataset);

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    SelectionToolbarAction& getSelectionToolbarAction() { return _selectionToolbarAction; }
    InteractionToolbarAction& getInteractionToolbarAction() { return _interactionToolbarAction; }
    SettingsAction& getSettingsAction() { return _settingsAction; }

private:
    LayersModel                 _layersModel;               /** Layers model */
    QItemSelectionModel         _selectionModel;            /** Layers selection model */
    QSplitter                   _splitter;                  /** Splitter which divides the layers view and editor */
    ImageViewerWidget           _imageViewerWidget;         /** Image viewer widget */
    DropWidget                  _dropWidget;                /** Widget for dropping data */
    SelectionToolbarAction      _selectionToolbarAction;    /** Toolbar action for selection */
    InteractionToolbarAction    _interactionToolbarAction;  /** Toolbar action for interaction */
    SettingsAction              _settingsAction;            /** Layers settings action */
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

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    ImageViewerPlugin* produce() override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const hdps::Datasets& datasets) const override;
};