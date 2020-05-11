#include "RootLayer.h"
#include "Renderer.h"

#include <QDebug>
#include <QOpenGLWidget>

RootLayer::RootLayer() :
	Layer("", Layer::Type::Group, "root", "Root", ult(Flag::Enabled)),
	Channels<std::uint8_t>(0)
{
}

void RootLayer::render(const QMatrix4x4& parentMVP)
{
	Layer::render(parentMVP);
}

void RootLayer::paint(QPainter* painter)
{
}

Qt::ItemFlags RootLayer::flags(const QModelIndex& index) const
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

QVariant RootLayer::data(const QModelIndex& index, const int& role) const
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

QModelIndexList RootLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
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

QSize RootLayer::imageSize() const
{
	return QSize();
}

Layer::Hints RootLayer::hints() const
{
	return Layer::hints();
}