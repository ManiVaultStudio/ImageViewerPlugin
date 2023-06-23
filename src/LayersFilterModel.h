#pragma once

#include <QSortFilterProxyModel>

/**
 * Layers filter model class
 *
 * Proxy model for filtering layers
 *
 * @author Thomas Kroes
 */
class LayersFilterModel final : public QSortFilterProxyModel
{
public:

    /** Use base constructor */
    using QSortFilterProxyModel::QSortFilterProxyModel;

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;
};
