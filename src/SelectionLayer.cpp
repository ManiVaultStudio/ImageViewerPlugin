#include "SelectionLayer.h"

#include <QDebug>

SelectionLayer::SelectionLayer(Layer* parent, Dataset* dataset, const QString& id, const QString& name, const int& flags) :
	Layer(parent, dataset, Layer::Type::Selection, id, name, flags)
{
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
}