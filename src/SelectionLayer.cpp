#include "SelectionLayer.h"

#include <QDebug>

SelectionLayer::SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const std::uint32_t& flags) :
	LayerItem(dataset, Type::Selection, id, name, flags)
{
}

Qt::ItemFlags SelectionLayer::itemFlags(const QModelIndex& index) const
{
	if (index.parent() == QModelIndex())
		return LayerItem::itemFlags(index);

	const auto column = static_cast<Column>(index.column());

	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (column) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, int role) const
{
	if (index.parent() == QModelIndex())
		return LayerItem::data(index, role);

	const auto column = static_cast<Column>(index.column());

	switch (column) {
		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (index.parent() == QModelIndex())
		return LayerItem::setData(index, value, role);
}