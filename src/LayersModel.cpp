#include "LayersModel.h"
#include "ImageViewerPlugin.h"
#include "Layer.h"
#include "RootLayer.h"
#include "GroupLayer.h"
#include "PointsLayer.h"
#include "SelectionLayer.h"

#include <QFont>
#include <QBrush>
#include <QDebug>
#include <QAbstractItemView>
#include <QMimeData>

LayersModel::LayersModel(ImageViewerPlugin* imageViewerPlugin) :
    QAbstractItemModel(imageViewerPlugin),
    _selectionModel(this),
    _root(nullptr)
{
}

LayersModel::~LayersModel()
{
    delete _root;
}

void LayersModel::paint(QPainter* painter)
{
    if (getSelectedLayer() == nullptr)
        return;

    getSelectedLayer()->paint(painter);
}

void LayersModel::dispatchEventToSelectedLayer(QEvent* event)
{
    const auto selectedRows = _selectionModel.selectedRows();

    if (selectedRows.isEmpty())
        return;

    getSelectedLayer()->handleEvent(event, selectedRows.first());
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    Q_UNUSED(parent);

    return ult(Layer::Column::End) + 1;
}

QVariant LayersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto layer = getLayer(index);

    return layer->getData(index, role);
}

QVariant LayersModel::data(const int& row, const int& column, const int& role) const
{
    return data(index(row, column), role);
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
{
    auto layer = getLayer(index);

    const auto affectedIndices = layer->setData(index, value, role);

    for (auto affectedIndex : affectedIndices) {
        emit dataChanged(affectedIndex, affectedIndex);
    }

    return true;
}

bool LayersModel::setData(const int& row, const int& column, const QVariant& value, int role /*= Qt::EditRole*/)
{
    return setData(index(row, column), value, role);
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
    return getLayer(index)->getFlags(index);
}

Qt::ItemFlags LayersModel::flags(const int& row, const int& column) const
{
    return flags(index(row, column));
}

Layer* LayersModel::getLayer(const QModelIndex& index) const
{
    if (index.isValid()) {
        auto layer = static_cast<Layer*>(index.internalPointer());
        
        if (layer)
            return layer;
    }

    return _root;
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return Layer::getColumnName(static_cast<Layer::Column>(section));
    }

    return QVariant();
}

QModelIndex LayersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    auto parentLayer = getLayer(parent);

    if (!parentLayer)
        return QModelIndex();

    auto childLayer = parentLayer->getChild(row);

    if (childLayer)
        return createIndex(row, column, childLayer);

    return QModelIndex();
}

bool LayersModel::insertLayer(int row, Layer* layer, const QModelIndex& parent /*= QModelIndex()*/)
{
    auto parentLayer = getLayer(parent);

    if (!parentLayer)
        return false;

    beginInsertRows(parent, row, row);
    {
        parentLayer->insertChild(row, layer);
    }
    endInsertRows();

    return true;
}

QModelIndex LayersModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childLayer     = getLayer(index);
    auto parentLayer    = childLayer ? childLayer->getParent() : nullptr;

    if (parentLayer == _root || !parentLayer)
        return QModelIndex();

    return createIndex(parentLayer->getChildIndex(), 0, parentLayer);
}

bool LayersModel::removeLayers(const QModelIndexList& indices)
{
    QModelIndexList sortedIndices = indices;

    qSort(sortedIndices.begin(), sortedIndices.end(), qGreater<QModelIndex>());

    for (auto sortedIndex : sortedIndices) {
        const int row = sortedIndex.row();

        beginRemoveRows(sortedIndex.parent(), row, row);
        {
            getLayer(sortedIndex.parent())->removeChild(row);
        }
        endRemoveRows();
    }

    return true;
}

bool LayersModel::mayMoveLayer(const QModelIndex& index, const int& delta) const
{
    const auto sourceIndex = index;
    const auto targetIndex = index.siblingAtRow(index.row() + delta);

    if (!sourceIndex.isValid() || !targetIndex.isValid())
        return false;

    return true;
}

bool LayersModel::moveLayer(const QModelIndex& sourceParent, const int& sourceRow, const QModelIndex& targetParent, int targetRow)
{
    if (targetRow < 0)
        targetRow = 0;

    if (sourceParent == targetParent) {
        if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
            auto sourceParentLayer  = getLayer(sourceParent);
            auto targetParentLayer  = getLayer(targetParent);
            auto sourceLayer        = sourceParentLayer->getChild(sourceRow);

            sourceParentLayer->removeChild(sourceRow, false);
            targetParentLayer->insertChild(targetRow > sourceRow ? targetRow - 1 : targetRow, sourceLayer);

            endMoveRows();
        }
    }
    else {
        if (beginMoveRows(sourceParent, sourceRow, sourceRow, targetParent, targetRow)) {
            auto sourceParentLayer  = getLayer(sourceParent);
            auto targetParentLayer  = getLayer(targetParent);
            auto sourceLayer        = sourceParentLayer->getChild(sourceRow);

            sourceParentLayer->removeChild(sourceRow, false);
            targetParentLayer->insertChild(targetRow, sourceLayer);

            endMoveRows();
        }
    }

    return true;
}

void LayersModel::initialize()
{
    _root = new RootLayer();
}

void LayersModel::addPointsDataset(const QString& datasetName)
{
    const auto selectionName            = QString("%1_selection").arg(datasetName);
    const auto selectionLayerIndices    = match(index(0, ult(Layer::Column::ID)), Qt::DisplayRole, selectionName, -1, Qt::MatchExactly);
    const auto createSelectionLayer     = selectionLayerIndices.isEmpty();
    const auto layerFlags               = ult(Layer::Flag::Enabled) | ult(Layer::Flag::Renamable);

    auto pointsLayer = new PointsLayer(datasetName, datasetName, datasetName, layerFlags);

    auto largestImageSize = QSize();

    for (auto imageLayerIndex : match(index(0, ult(Layer::Column::Type)), Qt::EditRole, ult(Layer::Type::Points), -1, Qt::MatchExactly | Qt::MatchRecursive)) {
        const auto imageSize = data(imageLayerIndex.siblingAtColumn(ult(Layer::Column::ImageSize)), Qt::EditRole).toSize();

        if (imageSize.width() > largestImageSize.width() && imageSize.height() > largestImageSize.height())
            largestImageSize = imageSize;
    }

    if (largestImageSize.isValid())
        pointsLayer->matchScaling(largestImageSize);

    if (pointsLayer->getImageCollectionType() == ult(ImageData::Type::Stack) && createSelectionLayer) {
        auto selectionLayer = new SelectionLayer(datasetName, selectionName, selectionName, layerFlags);

        selectionLayer->setOpacity(0.8f);

        if (largestImageSize.isValid())
            selectionLayer->matchScaling(largestImageSize);

        insertLayer(0, pointsLayer);
        insertLayer(0, selectionLayer);
        selectRow(1);
    }
    else {
        const auto row = selectionLayerIndices.isEmpty() ? 0 : selectionLayerIndices.first().row() + 1;

        insertLayer(row, pointsLayer);
        selectRow(row);
    }
}

void LayersModel::selectRow(const std::int32_t& row)
{
    _selectionModel.setCurrentIndex(index(row, 0), QItemSelectionModel::SelectionFlag::Current | QItemSelectionModel::SelectionFlag::ClearAndSelect | QItemSelectionModel::SelectionFlag::Rows);
}

Layer* LayersModel::getSelectedLayer()
{
    const auto selectedRows = _selectionModel.selectedRows();

    if (selectedRows.isEmpty())
        return nullptr;

    return getLayer(selectedRows.first());
}

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
    const auto parentLayer = getLayer(parent);

    return parentLayer ? parentLayer->getChildCount() : 0;
}