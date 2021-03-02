#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "StatusbarWidget.h"
#include "SettingsWidget.h"
#include "Layer.h"

#include <QDebug>
#include <QSplitter>

using namespace hdps;

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin() :
    ViewPlugin("Image Viewer"),
    _viewerWidget(nullptr),
    _statusbarWidget(nullptr),
    _settingsWidget(nullptr),
    _layersModel(this),
    _colorMapModel(this, ColorMap::Type::OneDimensional),
    _pointsDatasets()
{
    Layer::imageViewerPlugin = this;

    _viewerWidget   = new ViewerWidget(this);
    _settingsWidget = new SettingsWidget(this);

    setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);
}

void ImageViewerPlugin::init()
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    auto splitter = new QSplitter();

    auto viewerLayout = new QVBoxLayout();

    splitter->addWidget(_viewerWidget);
    splitter->addWidget(_settingsWidget);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    splitter->setCollapsible(1, true);

    layout->addWidget(splitter);

    _layersModel.initialize();

    /*
    auto pointsDataset = dynamic_cast<Points*>(&_core->requestData(dataset));

    if (pointsDataset != nullptr) {
        _pointsDatasets.insert(0, dataset);
        emit pointsDatasetsChanged(_pointsDatasets);
    }
    hdps::DataTypes supportedTypes;

    supportedTypes.append(PointType);

    return supportedTypes;
    */
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin();
}