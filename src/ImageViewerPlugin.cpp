#include "ImageViewerPlugin.h"
#include "ViewerWidget.h"
#include "StatusbarWidget.h"
#include "SettingsWidget.h"
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

Q_PLUGIN_METADATA(IID "nl.tudelft.ImageViewerPlugin")

/*
void LayersWidget::dragEnterEvent(QDragEnterEvent* dragEnterEvent)
{
    const auto items        = dragEnterEvent->mimeData()->text().split("\n");
    const auto datasetName  = items.at(0);
    const auto datasetType  = items.at(1);

    if (datasetType == "Points") {
        auto pointsDataset = _imageViewerPlugin->requestData<Points>(datasetName);

        if (pointsDataset.isDerivedData()) {
            auto sourcePointsDataset = hdps::DataSet::getSourceData<Points>(pointsDataset);

            if (sourcePointsDataset.getProperty("Type", "").toString() == "Images")
                dragEnterEvent->acceptProposedAction();
        }
        else {
            if (pointsDataset.getProperty("Type", "").toString() == "Images")
                dragEnterEvent->acceptProposedAction();
        }
    }

    if (datasetType == "Clusters")
        dragEnterEvent->acceptProposedAction();
}

void LayersWidget::dropEvent(QDropEvent* dropEvent)
{
    const auto items                    = dropEvent->mimeData()->text().split("\n");
    const auto datasetName              = items.at(0);
    const auto datasetType              = items.at(1);
    const auto selectionName            = QString("%1_selection").arg(datasetName);
    const auto selectionLayerIndices    = getLayersModel().match(getLayersModel().index(0, ult(Layer::Column::ID)), Qt::DisplayRole, selectionName, -1, Qt::MatchExactly);
    const auto createSelectionLayer     = selectionLayerIndices.isEmpty();
    const auto layerFlags               = ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable);

    auto largestImageSize = QSize();

    for (auto imageLayerIndex : getLayersModel().match(getLayersModel().index(0, ult(Layer::Column::Type)), Qt::EditRole, ult(Layer::Type::Points), -1, Qt::MatchExactly | Qt::MatchRecursive)) {
        const auto imageSize = getLayersModel().data(imageLayerIndex.siblingAtColumn(ult(Layer::Column::ImageSize)), Qt::EditRole).toSize();

        if (imageSize.width() > largestImageSize.width() && imageSize.height() > largestImageSize.height())
            largestImageSize = imageSize;
    }

    if (datasetType == "Points") {
        
    }

    if (datasetType == "Clusters") {
    }

    dropEvent->acceptProposedAction();
}
*/

ImageViewerPlugin::ImageViewerPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _viewerWidget(nullptr),
    _statusbarWidget(nullptr),
    _settingsWidget(nullptr),
    _layersModel(this),
    _colorMapModel(this, ColorMap::Type::OneDimensional),
    _pointsDatasets(),
    _dropWidget(nullptr)
{
    Layer::imageViewerPlugin = this;

    _viewerWidget   = new ViewerWidget(this);
    _settingsWidget = new SettingsWidget(this);
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
    splitter->addWidget(_settingsWidget);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    splitter->setCollapsible(1, true);

    layout->addWidget(splitter);

    _layersModel.initialize();
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
