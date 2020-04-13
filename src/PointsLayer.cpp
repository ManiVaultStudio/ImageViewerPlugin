#include "PointsLayer.h"
#include "PointsDataset.h"

#include <QDebug>

PointsLayer::PointsLayer(PointsDataset* pointsDataset, const QString& id, const QString& name, const int& flags) :
	LayerNode(pointsDataset, LayerNode::Type::Points, id, name, flags),
	_points(pointsDataset),
	_channels{-1,-1,-1},
	_noChannels(1),
	_size(),
	_square(true)
{
}

int PointsLayer::noColumns() const
{
	return ult(Column::End);
}

Qt::ItemFlags PointsLayer::flags(const QModelIndex& index) const
{
	auto flags = LayerNode::flags(index);

	switch (static_cast<Column>(index.column())) {
		case Column::Size:
			break;

		case Column::Width:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Height:
		{
			if (!_square)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Square:
			flags |= Qt::ItemIsEditable;
			break;
		
		case Column::Channel1:
			flags |= Qt::ItemIsEditable;
			break;

		case Column::Channel2:
		{
			if (_noChannels >= 2)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::Channel3:
		{
			if (_noChannels == 3)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Column::NoChannels:
		case Column::DimensionNames:
		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::Selection:
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return flags;
}

QVariant PointsLayer::data(const QModelIndex& index, const int& role) const
{
	if (isBaseLayerIndex(index))
		return LayerNode::data(index, role);

	switch (static_cast<Column>(index.column())) {
		case Column::Size:
			return size(role);

		case Column::Width:
			return width(role);

		case Column::Height:
			return height(role);

		case Column::Square:
			return square(role);

		case Column::Channel1:
			return channel(1, role);

		case Column::Channel2:
			return channel(2, role);

		case Column::Channel3:
			return channel(3, role);

		case Column::NoChannels:
			return noChannels(role);

		case Column::DimensionNames:
			return _points->dimensionNames(role);
			
		case Column::NoPoints:
			return _points->noPoints(role);

		case Column::NoDimensions:
			return _points->noDimensions(role);

		case Column::Selection:
			return _points->selection(role);

		case Column::SelectionSize:
			return _points->selectionSize(role);

		default:
			break;
	}

	return QVariant();
}

QModelIndexList PointsLayer::setData(const QModelIndex& index, const QVariant& value, const int& role)
{
	if (isBaseLayerIndex(index))
		return LayerNode::setData(index, value, role);

	QModelIndexList affectedIds({ index });

	switch (static_cast<Column>(index.column())) {
		case Column::Size:
			setSize(value.toSize());
			affectedIds << index.siblingAtColumn(ult(Column::Width)) << index.siblingAtColumn(ult(Column::Height));
			break;

		case Column::Width:
		{
			setWidth(value.toInt());

			if (_square) {
				setHeight(value.toInt());
				affectedIds << index.siblingAtColumn(ult(Column::Height));
			}
			
			break;
		}
			

		case Column::Height:
		{
			setHeight(value.toInt());

			if (_square) {
				setWidth(value.toInt());
				affectedIds << index.siblingAtColumn(ult(Column::Width));
			}
				
			break;
		}

		case Column::Square:
		{
			setSquare(value.toBool());

			affectedIds << index.siblingAtColumn(ult(Column::Width)) << index.siblingAtColumn(ult(Column::Height));

			break;
		}

		case Column::Channel1:
			setChannel(1, value.toInt());
			break;

		case Column::Channel2:
			setChannel(2, value.toInt());
			break;

		case Column::Channel3:
			setChannel(3, value.toInt());
			break;

		case Column::NoChannels:
		{
			setNoChannels(value.toInt());

			affectedIds << index.siblingAtColumn(ult(Column::Width)) << index.siblingAtColumn(ult(Column::Height));

			break;
		}

		case Column::NoPoints:
		case Column::NoDimensions:
		case Column::Selection:
		case Column::SelectionSize:
			break;

		default:
			break;
	}

	return affectedIds;
}

QVariant PointsLayer::size(const int& role /*= Qt::DisplayRole*/) const
{
	const auto sizeString = QString("%1x%2").arg(QString::number(_size.width()), QString::number(_size.height()));

	switch (role)
	{
		case Qt::DisplayRole:
			return sizeString;

		case Qt::EditRole:
			return _size;

		case Qt::ToolTipRole:
			return QString("Size: %1").arg(sizeString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setSize(const QSize& size)
{
	_size = size;
}

QVariant PointsLayer::width(const int& role /*= Qt::DisplayRole*/) const
{
	const auto widthString = QString::number(_size.width());

	switch (role)
	{
		case Qt::DisplayRole:
			return widthString;

		case Qt::EditRole:
			return _size.width();

		case Qt::ToolTipRole:
			return QString("Width: %1").arg(widthString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setWidth(const int& width)
{
	_size.setWidth(width);
}

QVariant PointsLayer::height(const int& role /*= Qt::DisplayRole*/) const
{
	const auto heightString = QString::number(_size.height());

	switch (role)
	{
		case Qt::DisplayRole:
			return heightString;

		case Qt::EditRole:
			return _size.height();

		case Qt::ToolTipRole:
			return QString("Height: %1").arg(heightString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setHeight(const int& height)
{
	_size.setHeight(height);
}

QVariant PointsLayer::square(const int& role /*= Qt::DisplayRole*/) const
{
	const auto squareString = _square ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return squareString;

		case Qt::EditRole:
			return _square;

		case Qt::ToolTipRole:
			return QString("Square: %1").arg(squareString);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setSquare(const bool& square)
{
	_square = square;

	if (_square)
		_size.setHeight(_size.width());
}

QVariant PointsLayer::channel(const int& channel, const int& role /*= Qt::DisplayRole*/) const
{
	const auto noChannel1String = QString::number(_channels[channel]);

	switch (role)
	{
		case Qt::DisplayRole:
			return noChannel1String;

		case Qt::EditRole:
			return _channels[channel];

		case Qt::ToolTipRole:
			return QString("Width: %1").arg(noChannel1String);

		default:
			break;
	}

	return QVariant();
}

void PointsLayer::setChannel(const int& channel, const int& dimension)
{
	_channels[channel] = dimension;
}

QVariant PointsLayer::noChannels(const int& role /*= Qt::DisplayRole*/) const
{
	return _noChannels;
}

void PointsLayer::setNoChannels(const int& noChannels)
{
	_noChannels = noChannels;
}