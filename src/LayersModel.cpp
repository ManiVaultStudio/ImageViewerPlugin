#include "LayersModel.h"

#include <Application.h>
#include <DataHierarchyItem.h>
#include <util/Exception.h>
#include <event/Event.h>
#include <PointData/PointData.h>

#include <QMessageBox>
#include <QPainter>

#include <stdexcept>

using namespace hdps;

LayersModel::LayersModel(QObject* parent) :
    QAbstractListModel(parent),
    _layers()
{
    _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataRemoved));

    // Register for events for points datasets
    _eventListener.registerDataEventByType(PointType, [this](DataEvent* dataEvent) {

        switch (dataEvent->getType())
        {
            case EventType::DataRemoved:
            {
                removeLayer(dataEvent->getDataset()->getGuid());
                break;
            }

            default:
                break;
        }
    });

    // Register for events for images datasets
    _eventListener.registerDataEventByType(ImageType, [this](DataEvent* dataEvent) {

        switch (dataEvent->getType())
        {
            case EventType::DataRemoved:
            {
                removeLayer(dataEvent->getDataset()->getGuid());
                break;
            }

            default:
                break;
        }
    });
}

LayersModel::~LayersModel()
{
    // Remove all layers
    for (std::int32_t row = 0; rowCount(); row++)
        removeLayer(row);
}

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return _layers.count();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    return Column::Count;
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex& parent /*= QModelIndex()*/) const
{
    return createIndex(row, column, static_cast<void*>(_layers.at(row)));
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
    auto layer = static_cast<Layer*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    break;

                case Column::Color:
                    return "";

                case Column::Name:
                    return data(index, Qt::EditRole).toString();

                case Column::ImageWidth:
                    return QString::number(data(index, Qt::EditRole).toInt());

                case Column::ImageHeight:
                    return QString::number(data(index, Qt::EditRole).toInt());

                case Column::Scale:
                case Column::Opacity:
                    return QString("%1%").arg(QString::number(data(index, Qt::EditRole).toFloat(), 'f', 1));

                default:
                    break;
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    return layer->getGeneralAction().getVisibleAction().isChecked();

                case Column::Color:
                    return layer->getGeneralAction().getColorAction().getColor();

                case Column::Name:
                    return layer->getImages()->getDataHierarchyItem().getFullPathName();

                case Column::ImageWidth:
                    return layer->getImageSize().width();

                case Column::ImageHeight:
                    return layer->getImageSize().height();

                case Column::Scale:
                    return layer->getGeneralAction().getScaleAction().getValue();

                case Column::Opacity:
                    return layer->getImageAction().getOpacityAction().getValue();

                default:
                    break;
            }

            break;
        }

        case Qt::CheckStateRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    return layer->getGeneralAction().getVisibleAction().isChecked() ? Qt::Checked : Qt::Unchecked;

                case Column::Color:
                case Column::Name:
                case Column::ImageWidth:
                case Column::ImageHeight:
                case Column::Scale:
                case Column::Opacity:
                    break;

                default:
                    break;
            }

            break;
        }

        case Qt::DecorationRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    break;

                case Column::Color:
                {
                    if (layer->getGeneralAction().getVisibleAction().isChecked()) {
                        return getColorIcon(layer->getGeneralAction().getColorAction().getColor());
                    } else {
                        const auto color = layer->getGeneralAction().getColorAction().getColor();
                        return getColorIcon(QColor::fromHsl(color.hue(), 0, color.lightness()));
                    }
                }

                case Column::Name:
                case Column::ImageWidth:
                case Column::ImageHeight:
                case Column::Scale:
                case Column::Opacity:
                    break;

                default:
                    break;
            }

            break;
        }
        
        case Qt::ForegroundRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                case Column::Color:
                case Column::Name:
                case Column::ImageWidth:
                case Column::ImageHeight:
                case Column::Scale:
                case Column::Opacity:
                    return QColor(layer->getGeneralAction().getVisibleAction().isChecked() ? QApplication::palette().color(QPalette::Text) : QApplication::palette().color(QPalette::Disabled, QPalette::Text));

                default:
                    break;
            }
        }
    }

    return QVariant();
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    auto layer = static_cast<Layer*>(index.internalPointer());

    switch (role) {
        case Qt::DisplayRole:
            break;

        case Qt::EditRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    break;

                case Column::Color:
                {
                    layer->getGeneralAction().getColorAction().setColor(value.value<QColor>());
                    break;
                }

                case Column::Name:
                {
                    layer->getGeneralAction().getNameAction().setString(value.toString());
                    break;
                }

                case Column::ImageWidth:
                case Column::ImageHeight:
                case Column::Scale:
                case Column::Opacity:
                    break;

                default:
                    break;
            }

            break;
        }

        case Qt::CheckStateRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                {
                    layer->getGeneralAction().getVisibleAction().setChecked(value.toBool());
                    break;
                }

                case Column::Color:
                case Column::Name:
                case Column::ImageWidth:
                case Column::ImageHeight:
                case Column::Scale:
                case Column::Opacity:
                    break;

                default:
                    break;
            }

            break;
        }
    }

    emit dataChanged(index, index);

    return true;
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    const auto iconSize = QSize(14, 14);

    if (orientation == Qt::Horizontal) {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Visible:
                    case Column::Color:
                        return "";

                    case Column::Name:
                        return "Name";

                    case Column::ImageWidth:
                        return "Width";

                    case Column::ImageHeight:
                        return "Height";

                    case Column::Scale:
                        return "Scale";

                    case Column::Opacity:
                        return "Opacity";

                    default:
                        break;
                }

                break;
            }

            case Qt::ToolTipRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Visible:
                        return "Whether the layer is visible or not";

                    case Column::Color:
                        return "Color of the layer";

                    case Column::Name:
                        return "Name of the layer";

                    case Column::ImageWidth:
                        return "Width of the image";

                    case Column::ImageHeight:
                        return "Height of the image";

                    case Column::Scale:
                        return "Scale of the layer";

                    case Column::Opacity:
                        return "Opacity of the layer";

                    default:
                        break;
                }

                break;
            }

            case Qt::DecorationRole:
            {
                switch (static_cast<Column>(section))
                {
                    case Column::Visible:
                        break;//return Application::getIconFont("FontAwesome").getIcon("eye", iconSize);

                    case Column::Color:
                        break;//return Application::getIconFont("FontAwesome").getIcon("palette", iconSize);

                    case Column::Name:
                        break;

                    case Column::ImageWidth:
                        break;//return Application::getIconFont("FontAwesome").getIcon("ruler-horizontal", QSize(12, 12));

                    case Column::ImageHeight:
                        break;//return Application::getIconFont("FontAwesome").getIcon("ruler-vertical", QSize(12, 12));

                    case Column::Scale:
                        break;//return Application::getIconFont("FontAwesome").getIcon("percentage", QSize(12, 12));

                    case Column::Opacity:
                        break;

                    default:
                        break;
                }

                break;
            }

            case Qt::TextAlignmentRole:
            {
                switch (static_cast<Column>(section)) {
                    case Column::Visible:
                        return Qt::AlignCenter;

                    case Column::Color:
                    case Column::Name:
                    case Column::ImageWidth:
                    case Column::ImageHeight:
                    case Column::Scale:
                    case Column::Opacity:
                        break;

                    default:
                        break;
                }

                break;
            }

            default:
                break;
        }
    }

    return QVariant();
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);

    auto layer = static_cast<Layer*>(index.internalPointer());

    if (index.column() == Column::Visible) {
        itemFlags |= Qt::ItemIsUserCheckable;
    }
    else {
        if (index.column() == Column::Name)
            itemFlags |= Qt::ItemIsEditable;
    }

    return itemFlags;
}

void LayersModel::addLayer(Layer* layer)
{
    try
    {
        // Insert the layer action at the beginning
        beginInsertRows(QModelIndex(), 0, 0);
        {
            // Insert the layer at the beginning (layer will be added on top of all other layers)
            _layers.insert(0, layer);

            // Inform views that the layer visibility has changed when it is changed in the action
            connect(&layer->getGeneralAction().getVisibleAction(), &ToggleAction::toggled, this, [this, layer](bool toggled) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Name);
                emit dataChanged(changedCell, changedCell.siblingAtColumn(Column::Last));
            });

            // Inform views that the layer color has changed when it is changed in the action
            connect(&layer->getGeneralAction().getColorAction(), &ColorAction::colorChanged, this, [this, layer](const QColor& color) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Color);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer name has changed when it is changed in the action
            connect(&layer->getGeneralAction().getNameAction(), &StringAction::stringChanged, this, [this, layer]() {
                const auto changedCell = index(_layers.indexOf(layer), Column::Name);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer scale has changed when it is changed in the action
            connect(&layer->getGeneralAction().getScaleAction(), &DecimalAction::valueChanged, this, [this, layer](const float& value) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Scale);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer opacity has changed when it is changed in the action
            connect(&layer->getImageAction().getOpacityAction(), &DecimalAction::valueChanged, this, [this, layer](const float& value) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Opacity);
                emit dataChanged(changedCell, changedCell);
            });
        }
        endInsertRows();
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
        // Get pointer to layer which needs to be removed
        auto removeLayer = _layers[row];

        qDebug() << "Remove layer:" << removeLayer->getGeneralAction().getNameAction().getString();

        // Remove the row from the model
        beginRemoveRows(QModelIndex(), row, row);
        {
            // Remove the layer from the list
            _layers.removeAt(row);

            // Remove the layer physically
            delete removeLayer;
        }
        endRemoveRows();
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
            throw std::runtime_error("Dataset UID is empty");

        for (const auto& layer : _layers) {
            if (layer->getImagesDatasetId() == datasetId)
                removeLayer(_layers.indexOf(layer));
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove layer from the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove layer from the layers model");
    }
}

void LayersModel::duplicateLayer(const QModelIndex& layerModelIndex)
{
    try
    {
        throw std::runtime_error("This feature is not yet implemented.");
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to duplicate the layer", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to duplicate the layer");
    }
}

void LayersModel::moveLayer(const QModelIndex& layerModelIndex, const std::int32_t& amount /*= 1*/)
{
    try
    {
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
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to move layer in the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to move layer in the layers model");
    }
}

QVector<Layer*>& LayersModel::getLayers()
{
    return _layers;
}

Layer& LayersModel::getLayerByDatasetId(const QString& datasetId)
{
    try
    {
        if (datasetId.isEmpty())
            throw std::runtime_error("Dataset UUID is empty");

        for (const auto& layer : _layers) {
            if (layer->getImagesDatasetId() == datasetId)
                return *layer;
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to get layer from the layers model", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to get layer from the layers model");
    }
}

QIcon LayersModel::getColorIcon(const QColor& color) const
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
