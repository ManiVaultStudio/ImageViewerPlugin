#pragma once

#include <QSortFilterProxyModel>

class LayersProxyModel : public QSortFilterProxyModel
{
public:
	
	/** TODO */
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
};