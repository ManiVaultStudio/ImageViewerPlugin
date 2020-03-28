#include "ProxyModel.h"
#include "Layer.h"

bool ProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
	const auto leftOrder	= source_left.siblingAtColumn(to_underlying(Layer::Column::Order)).data(Qt::EditRole).toInt();
	const auto rightOrder	= source_left.siblingAtColumn(to_underlying(Layer::Column::Order)).data(Qt::EditRole).toInt();

	return leftOrder < rightOrder;
}