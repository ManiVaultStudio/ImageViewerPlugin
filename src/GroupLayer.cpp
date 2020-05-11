#include "GroupLayer.h"

#include <QDebug>

GroupLayer::GroupLayer(const QString& id, const QString& name, const int& flags) :
	Layer("", Layer::Type::Group, id, name, flags),
	Channels<std::uint8_t>()
{
}

void GroupLayer::paint(QPainter* painter)
{
}

Qt::ItemFlags GroupLayer::flags(const QModelIndex& index) const
{
	auto flags = Layer::flags(index);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return flags;
}

QVariant GroupLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return Layer::data(index, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return QVariant();
}

QModelIndexList GroupLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	QModelIndexList affectedIds = Layer::setData(index, value, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return affectedIds;
}

Layer::Hints GroupLayer::hints() const
{
	return Layer::hints();
}