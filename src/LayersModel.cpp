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
    auto layerAction = _layers[index.row()];
    
    switch (role) {
        case Qt::DisplayRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Name:
                    return "Test";

                case Column::ImageWidth:
                    return "512";

                case Column::ImageHeight:
                    return "512";

                case Column::Scale:
                    return "100.0%";

                default:
                    break;
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(index.column())) {
                case Column::Name:
                    return "Test";

                case Column::ImageWidth:
                    return 512;

                case Column::ImageHeight:
                    return 512;

                case Column::Scale:
                    return 1.0f;

                default:
                    break;
            }

            break;
        }
    }

    return QVariant();
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
                        return Application::getIconFont("FontAwesome").getIcon("ruler-horizontal", QSize(12, 12));

                    case Column::ImageHeight:
                        return Application::getIconFont("FontAwesome").getIcon("ruler-vertical", QSize(12, 12));

                    case Column::Scale:
                        return Application::getIconFont("FontAwesome").getIcon("percentage", QSize(12, 12));

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

void LayersModel::addLayer(const SharedLayer& layer)
{
    try
    {
        // Insert the layer action at the end
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        {
            // Append the layer action
            _layers << layer;
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
