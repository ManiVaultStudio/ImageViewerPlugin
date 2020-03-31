#include "SelectionLayer.h"

#include <QDebug>

SelectionLayer::SelectionLayer(Dataset* dataset, const QString& id, const QString& name, const int& flags) :
	_Layer(dataset, _Layer::Type::Selection, id, name, flags)
{
}

Qt::ItemFlags SelectionLayer::flags(const QModelIndex& index) const
{
	if (!isSettingsIndex(index))
		return _Layer::flags(index);

	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant SelectionLayer::data(const QModelIndex& index, const int& role) const
{
	if (!isSettingsIndex(index))
		return _Layer::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

void SelectionLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (!isSettingsIndex(index))
		return _Layer::setData(index, value, role);
}