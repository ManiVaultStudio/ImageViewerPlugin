#include "LayersFilterModel.h"
#include "LayersModel.h"

#include <QDebug>

LayersFilterModel::LayersFilterModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
}

bool LayersFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;
}
