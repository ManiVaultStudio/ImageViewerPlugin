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

ImageViewerPlugin::ImageViewerPlugin() :
    ViewPlugin("Image Viewer"),
    _viewerWidget(nullptr),
    _statusbarWidget(nullptr),
    _settingsWidget(nullptr),
    _layersModel(this),
    _colorMapModel(this, ColorMap::Type::OneDimensional),
    _pointsDatasets(),
    _dropWidget(nullptr)
{
    Layer::imageViewerPlugin = this;
}

void ImageViewerPlugin::init()
{
	_viewerWidget = new ViewerWidget(this);
	_settingsWidget = new SettingsWidget(this);
	_dropWidget = new DropWidget(_viewerWidget);

	setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);

	_dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

	_dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
		DropWidget::DropRegions dropRegions;

		const auto mimeText = mimeData->text();
		const auto tokens = mimeText.split("\n");
		const auto datasetName = tokens[0];
		const auto dataType = DataType(tokens[1]);
		const auto dataTypes = DataTypes({ PointType });

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
    return new ImageViewerPlugin();
}