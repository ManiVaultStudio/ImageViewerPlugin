#include "LayersModel.h"
#include "Application.h"

#include <QMessageBox>

#include <stdexcept>

using namespace hdps;

LayersModel::LayersModel(QObject* parent) :
    QAbstractListModel(parent),
    _layers()
{
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

QVariant LayersModel::data(const QModelIndex &index, int role) const
{
    auto layer = _layers[index.row()];
    
    switch (role) {
        case Qt::DisplayRole:
        {
            switch (static_cast<Column>(index.column())) {
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
                case Column::Name:
                    return layer->getLayerAction().getGeneralAction().getNameAction().getString();

                case Column::ImageWidth:
                    return layer->getImageSize().width();

                case Column::ImageHeight:
                    return layer->getImageSize().height();

                case Column::Scale:
                    return layer->getLayerAction().getGeneralAction().getScaleAction().getValue();

                case Column::Opacity:
                    return layer->getLayerAction().getImageAction().getOpacityAction().getValue();

                default:
                    break;
            }

            break;
        }
    }

    return QVariant();
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    auto layer = _layers[index.row()];

    switch (role) {
        case Qt::DisplayRole:
            break;

        case Qt::EditRole:
        {
            switch (static_cast<Column>(index.column())) {
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
    }

    return true;
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (orientation == Qt::Horizontal) {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                switch (static_cast<Column>(section))
                {
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

            case Qt::DecorationRole:
            {
                switch (static_cast<Column>(section))
                {
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

    if (index.column() == Column::Name)
        itemFlags |= Qt::ItemIsEditable;

    return itemFlags;
}

void LayersModel::addLayer(const SharedLayer& layer)
{
    try
    {
        // Insert the layer action at the end
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        {
            // Append the layer action
            _layers << layer;

            connect(&layer->getLayerAction().getGeneralAction().getNameAction(), &StringAction::stringChanged, this, [this]() {
                const auto changedCell = index(rowCount() - 1, Column::Name);
                emit dataChanged(changedCell, changedCell);
            });

            connect(&layer->getLayerAction().getGeneralAction().getScaleAction(), &DecimalAction::valueChanged, this, [this](const float& value) {
                const auto changedCell = index(rowCount() - 1, Column::Scale);
                emit dataChanged(changedCell, changedCell);
            });

            connect(&layer->getLayerAction().getImageAction().getOpacityAction(), &DecimalAction::valueChanged, this, [this](const float& value) {
                const auto changedCell = index(rowCount() - 1, Column::Opacity);
                emit dataChanged(changedCell, changedCell);
            });
        }
        endInsertRows();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to add layer to the layers model", e.what());
    }
}

void LayersModel::removeLayer(const QModelIndex& index)
{
    try
    {
        // Remove the row
        beginRemoveRows(QModelIndex(), index.row(), index.row());
        {
            // Remove the layer action
            _layers.remove(index.row());
        }
        endRemoveRows();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to remove layer from the layers model", e.what());
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

                _layers.insert(rowIndices.last() + 1, _layers[rowIndices.first()]);
                _layers.removeAt(rowIndices.first());

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

                _layers.removeAt(rowIndices.first());
                _layers.insert(rowIndices.last(), cache);

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
