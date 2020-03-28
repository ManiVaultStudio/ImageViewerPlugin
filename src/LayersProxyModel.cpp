#include "LayersProxyModel.h"
#include "Layer.h"

bool LayersProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	const auto leftOrder	= left.siblingAtColumn(to_underlying(Layer::Column::Order)).data(Qt::EditRole).toInt();
	const auto rightOrder	= right.siblingAtColumn(to_underlying(Layer::Column::Order)).data(Qt::EditRole).toInt();

	return leftOrder < rightOrder;
}