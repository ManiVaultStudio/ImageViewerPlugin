#include "GroupLayer.h"

#include <QDebug>

GroupLayer::GroupLayer(const QString& id, const QString& name, const int& flags) :
	LayerNode("", LayerNode::Type::Group, id, name, flags)
{
}

Qt::ItemFlags GroupLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

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
		return LayerNode::data(index, role);

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
	QModelIndexList affectedIds = LayerNode::setData(index, value, role);

	/*
	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}
	*/

	return affectedIds;
}

void GroupLayer::mousePressEvent(QMouseEvent* mouseEvent)
{
}

void GroupLayer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
}

void GroupLayer::mouseMoveEvent(QMouseEvent* mouseEvent)
{
}

void GroupLayer::mouseWheelEvent(QWheelEvent* wheelEvent, const QModelIndex& index)
{
}

void GroupLayer::keyPressEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}

void GroupLayer::keyReleaseEvent(QKeyEvent* keyEvent, const QModelIndex& index)
{
}

void GroupLayer::paintEvent(QPaintEvent* paintEvent)
{

}
