#include "SelectionLayer.h"

#include <QDebug>

SelectionLayer::SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const int& flags) :
	Layer(dataset, Layer::Type::Selection, id, name, flags)
{
}

int SelectionLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

	if (index.column() == ult(Layer::Column::WindowNormalized) || index.column() == ult(Layer::Column::LevelNormalized))
		flags &= ~Qt::ItemIsEditable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (isBaseLayerIndex(index))
		return Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (isBaseLayerIndex(index))
		return Layer::setData(index, value, role);
}