#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "StatusbarWidget.h"
#include "Layer.h"

#include "ImageData/Images.h"
#include "ImageData/ImageData.h"
#include "widgets/DropWidget.h"
#include "util/DatasetRef.h"

#include <QDebug>
#include <QSplitter>
#include <QMimeData>

using namespace hdps;
using namespace hdps::gui;
using namespace hdps::util;

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin(hdps::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _layersModel(this),
    _dropWidget(nullptr),
    _settingsAction(this)
{
}

void ImageViewerPlugin::init()
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    auto splitter = new QSplitter();

    auto viewerLayout = new QVBoxLayout();

    auto viewerWidget = new QWidget();

    _dropWidget = new DropWidget(viewerWidget);

    splitter->addWidget(viewerWidget);
    splitter->addWidget(_settingsAction.createWidget(this));

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    splitter->setCollapsible(1, true);

    layout->addWidget(splitter);

    setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText = mimeData->text();
        const auto tokens = mimeText.split("\n");
        const auto datasetName = tokens[0];
        const auto dataType = DataType(tokens[1]);
        const auto dataTypes = DataTypes({ ImageType });

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == ImageType) {
            DatasetRef<Images> imagesDataset(datasetName);

            dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                _layersModel.addLayer();
            });
        }

        return dropRegions;
    });

    const auto updateDropIndicatorVisibility = [this]() -> void {
        _dropWidget->setShowDropIndicator(_layersModel.rowCount() == 0);
    };

    connect(&_layersModel, &QAbstractItemModel::rowsInserted, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });
    connect(&_layersModel, &QAbstractItemModel::rowsRemoved, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });
}

QIcon ImageViewerPluginFactory::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

ImageViewerPlugin* ImageViewerPluginFactory::produce()
{
    return new ImageViewerPlugin(this);
}

hdps::DataTypes ImageViewerPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    supportedTypes.append(ImageType);

    return supportedTypes;
}
