#pragma once

#include <QSortFilterProxyModel>

class ProxyModel : public QSortFilterProxyModel
{
public:
	
	/** TODO */
	bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};