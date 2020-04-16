#include "RootLayer.h"
#include "Renderer.h"

#include <QDebug>
#include <QOpenGLWidget>

RootLayer::RootLayer() :
	LayerNode("", LayerNode::Type::Group, "root", "Root", 0)
{
}

void RootLayer::render(const QMatrix4x4& parentMVP, const float& opacity)
{
	Renderable::renderer->bindOpenGLContext();

	glClearColor(0.1f, 0.5f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	LayerNode::render(parentMVP, opacity);

	//Renderable::renderer->zoomToRectangle(QRectF(-40, -40, 80, 80));
	Renderable::renderer->releaseOpenGLContext();

	//dynamic_cast<QOpenGLWidget*>(Renderable::renderer->parent())->update();
}

Qt::ItemFlags RootLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return flags;
}

QVariant RootLayer::data(const QModelIndex& index, const int& role) const
{
	if (index.column() < ult(Column::Start))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		default:
			break;
	}

	return QVariant();
}

QModelIndexList RootLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (index.column() < ult(Column::Start))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIndices{ index };

	return affectedIndices;
}