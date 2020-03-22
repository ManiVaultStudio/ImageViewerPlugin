#include "Layer.h"
#include "Dataset.h"

#include <QFont>
#include <QDebug>

Layer::Layer(Dataset* dataset, const QString& id /*= ""*/, const QString& name /*= ""*/, const LayerType& type /*= Type::Image*/, const std::uint32_t& flags) :
	QObject(dataset),
	_general(dataset, id, name, type, flags),
	_points(dataset),
	_images(dataset),
	_clusters(dataset)
{
}

int LayerColumnCount()
{
	return static_cast<int>(LayerColumn::End);
}

QVariant Layer::headerData(int section, Qt::Orientation orientation, int role)
{
	if (orientation == Qt::Horizontal) {
		return layerColumnName(static_cast<LayerColumn>(section));
	}

	return QVariant();
}

Qt::ItemFlags Layer::itemFlags(const LayerColumn& column) const
{
	if (column > LayerColumn::GeneralStart && column < LayerColumn::GeneralEnd)
		return _general.itemFlags(column);

	/*
	if (column > LayerColumn::SelectionStart && column < LayerColumn::SelectionEnd)
		return _selection.itemFlags(column);
	*/

	if (column > LayerColumn::PointsStart && column < LayerColumn::PointsEnd)
		return _points.itemFlags(column);

	if (column > LayerColumn::ImagesStart && column < LayerColumn::ImagesEnd)
		return _images.itemFlags(column);

	if (column > LayerColumn::ClustersStart && column < LayerColumn::ClustersEnd)
		return _clusters.itemFlags(column);

	return Qt::NoItemFlags;
}

QVariant Layer::data(const LayerColumn& column, int role) const
{
	if (column > LayerColumn::GeneralStart && column < LayerColumn::GeneralEnd)
		return _general.data(column, role);

	/*
	if (column > LayerColumn::SelectionStart && column < LayerColumn::SelectionEnd)
		return _selection.data(column, role);
	*/

	if (column > LayerColumn::PointsStart && column < LayerColumn::PointsEnd)
		return _points.data(column, role);

	if (column > LayerColumn::ImagesStart && column < LayerColumn::ImagesEnd)
		return _images.data(column, role);

	if (column > LayerColumn::ClustersStart && column < LayerColumn::ClustersEnd)
		return _clusters.data(column, role);

	return QVariant();
}

void Layer::setData(const LayerColumn& column, const QVariant& value, const int& role)
{
	if (column > LayerColumn::GeneralStart && column < LayerColumn::GeneralEnd)
		return _general.setData(column, value, role);

	/*
	if (column > LayerColumn::SelectionStart && column < LayerColumn::SelectionEnd)
		return _selection.data(column, value, role);
	*/

	if (column > LayerColumn::PointsStart && column < LayerColumn::PointsEnd)
		return _points.setData(column, value, role);

	if (column > LayerColumn::ImagesStart && column < LayerColumn::ImagesEnd)
		return _images.setData(column, value, role);

	if (column > LayerColumn::ClustersStart && column < LayerColumn::ClustersEnd)
		return _clusters.setData(column, value, role);
}