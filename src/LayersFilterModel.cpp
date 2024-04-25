#include "LayersFilterModel.h"

#include <QDebug>

bool LayersFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    const auto index = sourceModel()->index(row, 0, parent);

    if (!index.isValid())
        return true;

    if (filterRegularExpression().isValid()) {
        const auto key = sourceModel()->data(index.siblingAtColumn(static_cast<int>(filterKeyColumn())), filterRole()).toString();

        if (!key.contains(filterRegularExpression()))
            return false;
    }

    return true;
}

bool LayersFilterModel::lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const
{
    return lhs.data().toString() < rhs.data().toString();
}
