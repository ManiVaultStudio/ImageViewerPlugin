#include "LayersModel.h"

#include <QMessageBox>

#include <stdexcept>

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
                    return "1.0";

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

void LayersModel::addLayer(const SharedLayer& layer)
{
    try
    {
        // Insert the layer action at the end
        beginInsertRows(QModelIndex(), rowCount(), rowCount() + 1);
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

void LayersModel::removeLayer(const SharedLayer& layer)
{
    try
    {
        // Except if layer action is not found
        if (!_layers.contains(layer))
            throw std::runtime_error(QString("%1 not found").arg(layer->text()).toLatin1());

        // Get the row index of the item that needs to be removed
        const auto rowIndex = _layers.indexOf(layer);

        // Remove the row
        beginRemoveRows(QModelIndex(), rowCount(), rowCount() + 1);
        {
            // Remove the layer action
            _layers.removeOne(layerAction);
        }
        endRemoveRows();
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "Unable to remove layer from the layers model", e.what());
    }
}
