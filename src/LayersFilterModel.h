#pragma once

#include <QSortFilterProxyModel>

/**
 * Layers filter model class
 *
 * A class for filtering a layers model
 *
 * @author Thomas Kroes
 */
class LayersFilterModel : public QSortFilterProxyModel {
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    LayersFilterModel(QObject* parent);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

protected:
};