#include "RootLayer.h"
#include "Renderer.h"

#include <QDebug>
#include <QOpenGLWidget>

RootLayer::RootLayer() :
	LayerNode("", LayerNode::Type::Group, "root", "Root", ult(Flag::Enabled))
{
}

void RootLayer::render(const QMatrix4x4& parentMVP)
{
	LayerNode::render(parentMVP);
}

Qt::ItemFlags RootLayer::flags(const QModelIndex& index) const
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

QVariant RootLayer::data(const QModelIndex& index, const int& role) const
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

QModelIndexList RootLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
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