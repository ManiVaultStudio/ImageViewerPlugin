#include "Layer.h"
#include "Dataset.h"

#include "GeneralSettings.h"
#include "PointsSettings.h"
#include "ImagesSettings.h"
#include "ClustersSettings.h"

#include <QFont>
#include <QDebug>

Layer::Layer(Dataset* dataset, const QString& id /*= ""*/, const QString& name /*= ""*/, const Type& type /*= Type::Image*/, const std::uint32_t& flags) :
	QObject(dataset),
	_general(new GeneralSettings(this, dataset, id, name, type, flags)),
	_points(new PointsSettings(this, dataset)),
	_images(new ImagesSettings(this, dataset)),
	_clusters(new ClustersSettings(this, dataset))
{
	
}

int Layer::columnCount()
{
	return static_cast<int>(Layer::Column::GeneralEnd);
}

QVariant Layer::headerData(int section, Qt::Orientation orientation, int role)
{
	if (orientation == Qt::Horizontal) {
		return columnName(static_cast<Column>(section));
	}

	return QVariant();
}

Qt::ItemFlags Layer::itemFlags(const Column& column) const
{
	if (column > Column::GeneralStart && column < Column::GeneralEnd)
		return _general->itemFlags(column);

	/*
	if (column > Column::SelectionStart && column < Column::SelectionEnd)
		return _selection.itemFlags(column);
	*/

	if (column > Column::PointsStart && column < Column::PointsEnd)
		return _points->itemFlags(column);

	if (column > Column::ImagesStart && column < Column::ImagesEnd)
		return _images->itemFlags(column);

	if (column > Column::ClustersStart && column < Column::ClustersEnd)
		return _clusters->itemFlags(column);

	return Qt::NoItemFlags;
}

QVariant Layer::data(const Column& column, int role) const
{
	if (column > Column::GeneralStart && column < Column::GeneralEnd)
		return _general->data(column, role);

	/*
	if (column > Column::SelectionStart && column < Column::SelectionEnd)
		return _selection.data(column, role);
	*/

	if (column > Column::PointsStart && column < Column::PointsEnd)
		return _points->data(column, role);

	if (column > Column::ImagesStart && column < Column::ImagesEnd)
		return _images->data(column, role);

	if (column > Column::ClustersStart && column < Column::ClustersEnd)
		return _clusters->data(column, role);

	return QVariant();
}

void Layer::setData(const Column& column, const QVariant& value, const int& role)
{
	if (column > Column::GeneralStart && column < Column::GeneralEnd)
		return _general->setData(column, value, role);

	/*
	if (column > Column::SelectionStart && column < Column::SelectionEnd)
		return _selection.data(column, value, role);
	*/

	if (column > Column::PointsStart && column < Column::PointsEnd)
		return _points->setData(column, value, role);

	if (column > Column::ImagesStart && column < Column::ImagesEnd)
		return _images->setData(column, value, role);

	if (column > Column::ClustersStart && column < Column::ClustersEnd)
		return _clusters->setData(column, value, role);
}