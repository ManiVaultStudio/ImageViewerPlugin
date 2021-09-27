#include "ImageViewerPlugin.h"
#include "SettingsAction.h"
#include "ToolBarAction.h"
#include "Layer.h"

#include "ImageData/Images.h"
#include "ImageData/ImageData.h"
#include "widgets/DropWidget.h"
#include "util/DatasetRef.h"

#include <QDebug>
#include <QSplitter>
#include <QMimeData>

using namespace hdps::gui;
using namespace hdps::util;

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageViewerPlugin")

ImageViewerPlugin::ImageViewerPlugin(hdps::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _model(this),
    _selectionModel(&_model),
    _dropWidget(nullptr),
    _mainWidget(nullptr),
    _imageViewerWidget(nullptr),
    _settingsAction(nullptr),
    _toolBarAction(nullptr)
{
    setFocusPolicy(Qt::ClickFocus);
}

void ImageViewerPlugin::init()
{
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);

    auto splitter       = new QSplitter();
    auto viewerLayout   = new QVBoxLayout();

    _mainWidget         = new QWidget();
    _imageViewerWidget  = new ImageViewerWidget(this, _model);
    _settingsAction     = new SettingsAction(*this);
    _toolBarAction      = new ToolBarAction(*this);

    _imageViewerWidget->setAcceptDrops(true);

    _dropWidget = new DropWidget(_imageViewerWidget);

    auto mainWidgetLayout = new QVBoxLayout();

    // Configure main layout
    mainWidgetLayout->setMargin(0);
    mainWidgetLayout->setSpacing(0);

    // And add the toolbar and image viewer widget
    mainWidgetLayout->addWidget(_toolBarAction->createWidget(this));
    mainWidgetLayout->addWidget(_imageViewerWidget, 1);

    // Apply layout to main widget
    _mainWidget->setLayout(mainWidgetLayout);

    splitter->addWidget(_mainWidget);
    splitter->addWidget(_settingsAction->createWidget(this));

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    splitter->setCollapsible(1, true);

    layout->addWidget(splitter);

    setDockingLocation(hdps::gui::DockableWidget::DockingLocation::Right);

    _dropWidget->setDropIndicatorWidget(new DropWidget::DropIndicatorWidget(this, "No data loaded", "Drag an item from the data hierarchy and drop it here to visualize data..."));

    _dropWidget->initialize([this](const QMimeData* mimeData) -> DropWidget::DropRegions {
        DropWidget::DropRegions dropRegions;

        const auto mimeText     = mimeData->text();
        const auto tokens       = mimeText.split("\n");
        const auto datasetName  = tokens[0];
        const auto dataType     = hdps::DataType(tokens[1]);
        const auto dataTypes    = hdps::DataTypes({ ImageType });

        if (!dataTypes.contains(dataType))
            dropRegions << new DropWidget::DropRegion(this, "Incompatible data", "This type of data is not supported", false);

        if (dataType == ImageType) {
            dropRegions << new DropWidget::DropRegion(this, "Images", QString("Add an image layer for %1").arg(datasetName), true, [this, datasetName]() {
                try
                {
                    _model.addLayer(SharedLayer::create(*this, datasetName));
                }
                catch (std::exception& e)
                {
                    QMessageBox::critical(nullptr, QString("Unable to load '%1'").arg(datasetName).toLatin1(), e.what());
                }
            });
        }

        return dropRegions;
    });

    const auto updateDropIndicatorVisibility = [this]() -> void {
        _dropWidget->setShowDropIndicator(_model.rowCount() == 0);
    };

    connect(&_model, &QAbstractItemModel::rowsInserted, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, [updateDropIndicatorVisibility]() { updateDropIndicatorVisibility(); });

    connect(_imageViewerWidget, &ImageViewerWidget::pixelSelectionStarted, this, [this]() {

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Publish the selection
        layer->startSelection();
    });

    connect(_imageViewerWidget, &ImageViewerWidget::mousePositionsChanged, this, [this](const QVector<QPoint>& mousePositions) {

        // No point in computing selection when there are no mouse positions
        if (mousePositions.isEmpty())
            return;

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Compute the layer selection
        layer->computeSelection(mousePositions);

        // Publish the selection if notifications during selection are turned on
        if (_settingsAction->getSelectionAction().getNotifyDuringSelectionAction().isChecked())
            layer->publishSelection();
    });

    connect(_imageViewerWidget, &ImageViewerWidget::pixelSelectionEnded, this, [this]() {

        // Get selected layers model rows
        const auto selectedRows = _selectionModel.selectedRows();

        // Only compute selection when one layer is selected
        if (selectedRows.count() != 1)
            return;

        // Get pointer to layer from the selected model index
        auto layer = static_cast<Layer*>(selectedRows.first().internalPointer());

        // Publish the selection
        layer->publishSelection();
    });
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
    hdps::DataTypes supportedTypes;

    supportedTypes.append(ImageType);

    return supportedTypes;
}
