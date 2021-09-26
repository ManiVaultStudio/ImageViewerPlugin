#include "LayersModel.h"
#include "Application.h"

#include <QMessageBox>
#include <QPainter>

#include <stdexcept>

using namespace hdps;

LayersModel::LayersModel(QObject* parent) :
    QAbstractListModel(parent),
    hdps::EventListener(),
    _layers()
{
    setEventCore(Application::core());

    registerDataEventByType(ImageType, [this](DataEvent* dataEvent) {
        switch (dataEvent->getType())
        {
            case EventType::DataAboutToBeRemoved:
            {
                removeLayer(dataEvent->dataSetName);
                break;
            }

            default:
                break;
        }
    });
}

LayersModel::~LayersModel()
{
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
    return createIndex(row, column, static_cast<void*>(_layers.at(row).get()));
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
    auto layer          = static_cast<Layer*>(index.internalPointer());
    auto& layerAction   = layer->getLayerAction();

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
                    return layerAction.getGeneralAction().getVisibleAction().isChecked();

                case Column::Color:
                    return layerAction.getGeneralAction().getColorAction().getColor();

                case Column::Name:
                    return layerAction.getGeneralAction().getNameAction().getString();

                case Column::ImageWidth:
                    return layer->getImageSize().width();

                case Column::ImageHeight:
                    return layer->getImageSize().height();

                case Column::Scale:
                    return layerAction.getGeneralAction().getScaleAction().getValue();

                case Column::Opacity:
                    return layerAction.getImageAction().getOpacityAction().getValue();

                default:
                    break;
            }

            break;
        }

        case Qt::CheckStateRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Visible:
                    return layerAction.getGeneralAction().getVisibleAction().isChecked() ? Qt::Checked : Qt::Unchecked;

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
                    if (layerAction.getGeneralAction().getVisibleAction().isChecked()) {
                        return getColorIcon(layerAction.getGeneralAction().getColorAction().getColor());
                    } else {
                        const auto color = layerAction.getGeneralAction().getColorAction().getColor();
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
                    return QColor(layerAction.getGeneralAction().getVisibleAction().isChecked() ? QApplication::palette().color(QPalette::Text) : QApplication::palette().color(QPalette::Disabled, QPalette::Text));

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
                    layer->getLayerAction().getGeneralAction().getColorAction().setColor(value.value<QColor>());
                    break;
                }

                case Column::Name:
                {
                    layer->getLayerAction().getGeneralAction().getNameAction().setString(value.toString());
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
                    layer->getLayerAction().getGeneralAction().getVisibleAction().setChecked(value.toBool());
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

void LayersModel::addLayer(const SharedLayer& layer)
{
    try
    {
        // Insert the layer action at the beginning
        beginInsertRows(QModelIndex(), 0, 0);
        {
            // Insert the layer at the beginning (layer will be added on top of all other layers)
            _layers.insert(0, layer);

            // Inform views that the layer visibility has changed when it is changed in the action
            connect(&layer->getLayerAction().getGeneralAction().getVisibleAction(), &ToggleAction::toggled, this, [this, layer](bool toggled) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Name);
                emit dataChanged(changedCell, changedCell.siblingAtColumn(Column::Last));
            });

            // Inform views that the layer color has changed when it is changed in the action
            connect(&layer->getLayerAction().getGeneralAction().getColorAction(), &ColorAction::colorChanged, this, [this, layer](const QColor& color) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Color);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer name has changed when it is changed in the action
            connect(&layer->getLayerAction().getGeneralAction().getNameAction(), &StringAction::stringChanged, this, [this, layer]() {
                const auto changedCell = index(_layers.indexOf(layer), Column::Name);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer scale has changed when it is changed in the action
            connect(&layer->getLayerAction().getGeneralAction().getScaleAction(), &DecimalAction::valueChanged, this, [this, layer](const float& value) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Scale);
                emit dataChanged(changedCell, changedCell);
            });

            // Inform views that the layer opacity has changed when it is changed in the action
            connect(&layer->getLayerAction().getImageAction().getOpacityAction(), &DecimalAction::valueChanged, this, [this, layer](const float& value) {
                const auto changedCell = index(_layers.indexOf(layer), Column::Opacity);
                emit dataChanged(changedCell, changedCell);
            });

            layer->zoomToExtents();
        }
        endInsertRows();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to add layer to the layers model", e.what());
    }
}

void LayersModel::removeLayer(const QModelIndex& layerModelIndex)
{
    try
    {
        // Remove the row
        beginRemoveRows(QModelIndex(), layerModelIndex.row(), layerModelIndex.row());
        {
            // Remove the layer action
            _layers.remove(layerModelIndex.row());
        }
        endRemoveRows();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to remove layer from the layers model", e.what());
    }
}

void LayersModel::removeLayer(const QString& datasetName)
{
    try
    {
        if (datasetName.isEmpty())
            throw std::runtime_error("Cannot remove layer with empty name");

        for (const auto& layer : _layers) {
            if (datasetName == layer->getImagesDatasetName())
                removeLayer(index(_layers.indexOf(layer), 0));
        }
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to remove layer from the layers model", e.what());
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
        QMessageBox::critical(nullptr, "Unable to duplicate the layer", e.what());
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
        QMessageBox::critical(nullptr, "Unable to move layer in the layers model", e.what());
    }
}

QVector<SharedLayer>& LayersModel::getLayers()
{
    return _layers;
}

QIcon LayersModel::getColorIcon(const QColor& color) const
{
    QPixmap pixmap(QSize(14, 14));

    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    const auto radius = 3;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(color));
    painter.drawRoundedRect(0, 0, 14, 14, radius, radius);

    return QIcon(pixmap);
}
