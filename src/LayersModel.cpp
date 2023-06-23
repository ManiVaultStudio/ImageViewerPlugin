#include "LayersModel.h"
#include "ImageViewerPlugin.h"

#include <Application.h>
#include <CoreInterface.h>
#include <DataHierarchyItem.h>
#include <util/Exception.h>
#include <event/Event.h>
#include <PointData/PointData.h>

#include <QMessageBox>
#include <QPainter>

#include <stdexcept>

using namespace hdps;
using namespace hdps::util;

LayersModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant LayersModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return _columHeaderInfo._display;

        case Qt::EditRole:
            return _columHeaderInfo._edit;

        case Qt::ToolTipRole:
            return _columHeaderInfo._tooltip;

        default:
            break;
    }

    return QVariant();
}

LayersModel::Item::Item(Layer* layer, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _layer(layer)
{
    Q_ASSERT(_layer != nullptr);

    setEditable(editable);
    setDropEnabled(true);

    connect(&getLayer()->getGeneralAction().getVisibleAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        emitDataChanged();
    });
}

QPointer<Layer> LayersModel::Item::getLayer() const
{
    return _layer;
}

QVariant LayersModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::ForegroundRole:
            return QColor(getLayer()->getGeneralAction().getVisibleAction().isChecked() ? QApplication::palette().color(QPalette::Text) : QApplication::palette().color(QPalette::Disabled, QPalette::Text));

        default:
            break;
    }

    return QStandardItem::data(role);
}

LayersModel::VisibleItem::VisibleItem(Layer* layer) :
    Item(layer)
{
    setCheckable(true);
    setCheckState(getLayer()->getGeneralAction().getVisibleAction().isChecked() ? Qt::Checked : Qt::Unchecked);
}

QVariant LayersModel::VisibleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getGeneralAction().getVisibleAction().isChecked();

        case Qt::ToolTipRole:
            return QString("Layer is: %1").arg(data(Qt::EditRole).toBool() ? "visible" : "hidden");

        case Qt::CheckStateRole:
            return data(Qt::EditRole).toBool() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;

        default:
            break;
    }

    return Item::data(role);
}

void LayersModel::VisibleItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::CheckStateRole:
            getLayer()->getGeneralAction().getVisibleAction().setChecked(value.toBool());
            break;

        default:
            Item::setData(value, role);
    }
}

LayersModel::ColorItem::ColorItem(Layer* layer) :
    Item(layer)
{
    connect(&getLayer()->getGeneralAction().getColorAction(), &ColorAction::colorChanged, this, [this](const QColor& color) -> void {
        emitDataChanged();
    });
}

QVariant LayersModel::ColorItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getGeneralAction().getColorAction().getColor();

        case Qt::ToolTipRole:
        {
            const auto color = data(Qt::EditRole).value<QColor>();

            return QString("Color: rgb(%1, %2, %3)").arg(color.red(), color.green(), color.blue());
        }

        case Qt::DecorationRole:
        {
            const auto color = getLayer()->getGeneralAction().getColorAction().getColor();

            if (getLayer()->getGeneralAction().getVisibleAction().isChecked())
                return getColorIcon(color);
            else
                return getColorIcon(QColor::fromHsl(color.hue(), 0, color.lightness()));
        }

        default:
            break;
    }

    return Item::data(role);
}

void LayersModel::ColorItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getLayer()->getGeneralAction().getColorAction().setColor(value.value<QColor>());
            break;

        default:
            Item::setData(value, role);
    }
}

QIcon LayersModel::ColorItem::getColorIcon(const QColor& color) const
{
    QPixmap pixmap(QSize(13, 13));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    const auto radius = 3;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(color));
    painter.drawRoundedRect(0, 0, pixmap.width(), pixmap.height(), radius, radius);

    return QIcon(pixmap);
}

LayersModel::NameItem::NameItem(Layer* layer) :
    Item(layer)
{
    setEditable(true);

    connect(&getLayer()->getGeneralAction().getNameAction(), &StringAction::stringChanged, this, [this](const QString& string) -> void {
        emitDataChanged();
    });
}

QVariant LayersModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getLayer()->getGeneralAction().getNameAction().getString();

        case Qt::ToolTipRole:
            return QString("Name: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

void LayersModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getLayer()->getGeneralAction().getNameAction().setString(value.toString());
            break;

        default:
            Item::setData(value, role);
    }
}

LayersModel::DatasetNameItem::DatasetNameItem(Layer* layer) :
    Item(layer)
{
    connect(&getLayer()->getGeneralAction().getDatasetNameAction(), &StringAction::stringChanged, this, [this](const QString& string) -> void {
        emitDataChanged();
    });
}

QVariant LayersModel::DatasetNameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getLayer()->getGeneralAction().getDatasetNameAction().getString();

        case Qt::ToolTipRole:
            return QString("Dataset name: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LayersModel::DatasetIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getLayer()->getImages()->getDataHierarchyItem().getDataset().getDatasetId();

        case Qt::ToolTipRole:
            return QString("Dataset ID: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LayersModel::WidthItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getImageSize().width();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return QString("Image width: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant LayersModel::HeightItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getImageSize().height();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return QString("Image height: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

LayersModel::ScaleItem::ScaleItem(Layer* layer) :
    Item(layer, true)
{
    connect(&getLayer()->getGeneralAction().getScaleAction(), &DecimalAction::valueChanged, this, [this](float value) -> void {
        emitDataChanged();
    });
}

QVariant LayersModel::ScaleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getGeneralAction().getScaleAction().getValue();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toFloat(), 'f', 1);

        case Qt::ToolTipRole:
            return QString("Scale: %1%%").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

void LayersModel::ScaleItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getLayer()->getGeneralAction().getScaleAction().setValue(value.toFloat());
            break;

        default:
            Item::setData(value, role);
    }
}

LayersModel::OpacityItem::OpacityItem(Layer* layer) :
    Item(layer, true)
{
    connect(&getLayer()->getImageSettingsAction().getOpacityAction(), &DecimalAction::valueChanged, this, [this](float value) -> void {
        emitDataChanged();
    });
}

QVariant LayersModel::OpacityItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getLayer()->getImageSettingsAction().getOpacityAction().getValue();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toFloat(), 'f', 1);

        case Qt::ToolTipRole:
            return QString("Opacity: %1%%").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

void LayersModel::OpacityItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getLayer()->getImageSettingsAction().getOpacityAction().setValue(value.toFloat());
            break;

        default:
            Item::setData(value, role);
    }
}

LayersModel::Row::Row(Layer* layer)
{
    append(new VisibleItem(layer));
    append(new ColorItem(layer));
    append(new NameItem(layer));
    append(new DatasetNameItem(layer));
    append(new DatasetIdItem(layer));
    append(new WidthItem(layer));
    append(new HeightItem(layer));
    append(new ScaleItem(layer));
    append(new OpacityItem(layer));
}

QMap<LayersModel::Column, LayersModel::ColumHeaderInfo> LayersModel::columnInfo = QMap<LayersModel::Column, LayersModel::ColumHeaderInfo>({
    { LayersModel::Column::Visible, { "" , "Visible", "Whether the visible or not" } },
    { LayersModel::Column::Color, { "" , "Color", "Layer color" } },
    { LayersModel::Column::Name, { "Name" , "Name", "Name of the layer" } },
    { LayersModel::Column::DatasetName, { "Dataset Name" , "Dataset Name", "Name of the dataset" } },
    { LayersModel::Column::DatasetId, { "Dataset ID",  "Dataset ID", "Globally unique identifier of the dataset" } },
    { LayersModel::Column::ImageWidth, { "Width",  "Width", "Width of the image" } },
    { LayersModel::Column::ImageHeight, { "Height",  "Height", "Height of the image" } },
    { LayersModel::Column::Scale, { "Scale", "Scale", "Scale of the image" } },
    { LayersModel::Column::Opacity, { "Opacity", "Opacity", "Layer transparency" } }
});

LayersModel::LayersModel(QObject* parent) :
    Serializable("Layers"),
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetRemoved));

    _eventListener.registerDataEventByType(PointType, [this](DataEvent* dataEvent) {

        switch (dataEvent->getType())
        {
            case EventType::DatasetRemoved:
                removeLayer(dataEvent->getDataset()->getId());
                break;

            default:
                break;
        }
    });

    _eventListener.registerDataEventByType(ImageType, [this](DataEvent* dataEvent) {

        switch (dataEvent->getType())
        {
            case EventType::DatasetRemoved:
                removeLayer(dataEvent->getDataset()->getId());
                break;

            default:
                break;
        }
    });
}

LayersModel::~LayersModel()
{
    for (std::int32_t row = 0; rowCount(); row++)
        removeLayer(row);
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = QAbstractItemModel::flags(index);

    if (index.column() == static_cast<int>(Column::Visible))
        itemFlags |= Qt::ItemIsUserCheckable;

    return itemFlags;
}

void LayersModel::addLayer(Layer* layer)
{
    try
    {
        Q_ASSERT(layer != nullptr);

        appendRow(Row(layer));

        static_cast<ImageViewerPlugin*>(parent())->getImageViewerWidget().updateWorldBoundingRectangle();

        if (rowCount() == 1)
            layer->zoomToExtents();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add layer to the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add layer to the layers model");
    }
}

void LayersModel::removeLayer(const std::uint32_t& row)
{
    try
    {
        removeRow(row, QModelIndex());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove layer from the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove layer from the layers model");
    }
}

void LayersModel::removeLayer(const QModelIndex& layerModelIndex)
{
    removeLayer(layerModelIndex.row());
}

void LayersModel::removeLayer(const QString& datasetId)
{
    try
    {
        if (datasetId.isEmpty())
            throw std::runtime_error("Dataset GUID is empty");

        const auto matches = match(index(0, static_cast<int>(Column::DatasetId), QModelIndex()), Qt::EditRole, datasetId, -1, Qt::MatchFlag::MatchRecursive);

        if (matches.isEmpty())
            throw std::runtime_error("Dataset GUID not found");

        removeLayer(matches.first().row());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove layer from the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove layer from the layers model");
    }
}

void LayersModel::moveLayer(const QModelIndex& layerModelIndex, const std::int32_t& amount /*= 1*/)
{
    try
    {
        /*
        // Establish source and target row index
        const auto sourceRowIndex = layerModelIndex.row();
        const auto targetRowIndex = std::clamp(sourceRowIndex + amount, 0, rowCount() - 1);

        QVector<std::int32_t> rowIndices{ sourceRowIndex, targetRowIndex };

        if (sourceRowIndex < targetRowIndex) {
            
            // Begin moving the row in the model
            if (beginMoveRows(QModelIndex(), rowIndices.first(), rowIndices.first(), QModelIndex(), rowIndices.last() + 1)) {

                // Re-arrange internal layers data
                _layers.insert(rowIndices.last() + 1, _layers[rowIndices.first()]);
                _layers.removeAt(rowIndices.first());

                // Finished moving row around
                endMoveRows();
            }
            else {
                throw std::runtime_error("Unable to begin moving rows");
            }
        }
        else {

            // Begin moving the row in the model
            if (beginMoveRows(QModelIndex(), rowIndices.first(), rowIndices.first(), QModelIndex(), rowIndices.last())) {

                auto cache = _layers[rowIndices.first()];

                // Re-arrange internal layers data
                _layers.removeAt(rowIndices.first());
                _layers.insert(rowIndices.last(), cache);

                // Finished moving row around
                endMoveRows();
            }
            else {
                throw std::runtime_error("Unable to begin moving rows");
            }
        }
        */
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to move layer in the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to move layer in the layers model");
    }
}

LayersModel::Layers LayersModel::getLayers() const
{
    Layers layers;

    for (int layerIndex = 0; layerIndex < rowCount(); layerIndex++)
        layers << static_cast<Item*>(itemFromIndex(index(layerIndex, 0)))->getLayer();

    return layers;
}

void LayersModel::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Layers");

    auto imageViewerPlugin = static_cast<ImageViewerPlugin*>(parent());

    for (auto layerVariant : variantMap["Layers"].toList()) {
        auto layer = new Layer(&imageViewerPlugin->getSettingsAction().getEditLayersAction(), layerVariant.toMap()["Title"].toString());

        layer->initialize(imageViewerPlugin, hdps::data().getSet(layerVariant.toMap()["DatasetId"].toString()));
        layer->fromVariantMap(layerVariant.toMap());
        layer->scaleToFit(imageViewerPlugin->getImageViewerWidget().getWorldBoundingRectangle(false));

        addLayer(layer);
    }

    imageViewerPlugin->getImageViewerWidget().update();
}

QVariantMap LayersModel::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList layers;

    for (auto layer : getLayers())
        layers.insert(0, layer->toVariantMap());

    variantMap.insert({
        { "Layers", layers }
    });

    return variantMap;
}

Layer* LayersModel::getLayerFromIndex(const QModelIndex& layerModelIndex) const
{
    return static_cast<LayersModel::Item*>(itemFromIndex(layerModelIndex))->getLayer();
}
