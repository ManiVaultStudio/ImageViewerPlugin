#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "StatusbarWidget.h"
#include "Layer.h"

#include <PointData.h>
#include <ClusterData.h>
#include <ColorData.h>
#include <widgets/DropWidget.h>

#include <QDebug>
#include <QSplitter>
#include <QMimeData>

using namespace hdps;
using namespace hdps::gui;

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _viewerWidget(nullptr),
    _statusbarWidget(nullptr),
    _layersModel(this),
    _pointsDatasets(),
    _dropWidget(nullptr),
    _settingsAction(this)
{
    Layer::imageViewerPlugin = this;

    _viewerWidget   = new ViewerWidget(this);
    _dropWidget     = new DropWidget(_viewerWidget);

    setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText     = mimeData->text();
        const auto tokens       = mimeText.split("\n");
        const auto datasetName  = tokens[0];
        const auto dataType     = DataType(tokens[1]);
        const auto dataTypes    = DataTypes({ PointType });

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == PointType) {
            auto pointsDataset = hdps::DataSet::getSourceData<Points>(_core->requestData<Points>(datasetName));

            if (pointsDataset.getProperty("Type", "").toString() == "Images") {
                dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                    _layersModel.addPointsDataset(datasetName);
                });
            }
        }

        return dropRegions;
    });

    const auto updateDropIndicatorVisibility = [this]() -> void {
        _dropWidget->setShowDropIndicator(_layersModel.rowCount() == 0);
    };

    connect(&_layersModel, &QAbstractItemModel::rowsInserted, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });
    connect(&_layersModel, &QAbstractItemModel::rowsRemoved, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });
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
    splitter->addWidget(_settingsAction.createWidget(this));

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    splitter->setCollapsible(1, true);

    layout->addWidget(splitter);

    _layersModel.initialize();
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
	supportedTypes.append(PointType);
	return supportedTypes;
}
