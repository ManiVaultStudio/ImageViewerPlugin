#include "LayersModel.h"
#include "DatasetsModel.h"

#include <QItemSelectionModel>
#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(ImageDataset* imageDataset) :
	QAbstractListModel(),
	_imageDataset(imageDataset)
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return layers().size();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 17;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= rowCount() || index.row() < 0)
		return QVariant();

	auto layer = layers().at(index.row());

	switch (role)
	{
		case Qt::FontRole:
		{
			switch (index.column()) {
				case Columns::Type:
					return layer->type(Qt::FontRole).toString();

				default:
					break;
			}

			break;
		}

		case Qt::ForegroundRole:
		{
			if (index.column() == Columns::Locked)
				return QBrush(Qt::black);
			else
				return layer->flag(Layer::Flags::Enabled, Qt::EditRole).toBool() ? QBrush(Qt::black) : QBrush(QColor(80, 80, 80));

			break;
		}

		case Qt::CheckStateRole:
		{
			switch (index.column()) {
				case Columns::Enabled:
					return layer->flag(Layer::Flags::Enabled, Qt::EditRole).toBool() ? Qt::Checked : Qt::Unchecked;
			}

			break;
		}

		case Qt::DisplayRole:
		{
			switch (index.column()) {
				case Columns::Enabled:
					break;

				case Columns::Type:
					return layer->type(Roles::FontIconText).toString();

				case Columns::Locked:
					return layer->flag(Layer::Flags::Frozen, Qt::EditRole).toBool() ? u8"\uf023" : u8"\uf09c";

				case Columns::ID:
					return layer->id(Qt::DisplayRole).toString();

				case Columns::Name:
					return layer->name(Qt::DisplayRole).toString();

				case Columns::Frozen:
					return layer->flag(Layer::Flags::Frozen, Qt::DisplayRole);

				case Columns::Removable:
					return layer->flag(Layer::Flags::Removable, Qt::DisplayRole);

				case Columns::Mask:
					return layer->flag(Layer::Flags::Mask, Qt::DisplayRole);

				case Columns::Renamable:
					return layer->flag(Layer::Flags::Renamable, Qt::DisplayRole);

				case Columns::Order:
					return layer->order(Qt::DisplayRole);

				case Columns::Opacity:
					return layer->opacity(Qt::DisplayRole);

				case Columns::WindowNormalized:
					return layer->windowNormalized(Qt::DisplayRole);

				case Columns::LevelNormalized:
					return layer->levelNormalized(Qt::DisplayRole);

				case Columns::Color:
					return layer->color(Qt::DisplayRole);

				case Columns::Image:
					return layer->image(Qt::DisplayRole);

				case Columns::ImageRange:
					return layer->imageRange(Qt::DisplayRole);

				case Columns::DisplayRange:
					return layer->displayRange(Qt::DisplayRole);

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (index.column()) {
				case Columns::Enabled:
					return layer->flag(Layer::Flags::Enabled, Qt::EditRole);

				case Columns::Type:
					return layer->type(Qt::EditRole);

				case Columns::Locked:
					return layer->flag(Layer::Flags::Frozen, Qt::EditRole);

				case Columns::ID:
					return layer->id(Qt::EditRole);

				case Columns::Name:
					return layer->name(Qt::EditRole);

				case Columns::Frozen:
					return layer->flag(Layer::Flags::Frozen, Qt::EditRole);

				case Columns::Removable:
					return layer->flag(Layer::Flags::Removable, Qt::EditRole);

				case Columns::Mask:
					return layer->flag(Layer::Flags::Mask, Qt::EditRole);

				case Columns::Renamable:
					return layer->flag(Layer::Flags::Renamable, Qt::EditRole);

				case Columns::Order:
					return layer->order(Qt::EditRole);

				case Columns::Opacity:
					return layer->opacity(Qt::EditRole);

				case Columns::WindowNormalized:
					return layer->windowNormalized(Qt::EditRole);

				case Columns::LevelNormalized:
					return layer->levelNormalized(Qt::EditRole);

				case Columns::Color:
					return layer->color(Qt::EditRole);

				case Columns::Image:
					return layer->image(Qt::EditRole);

				case Columns::ImageRange:
					return layer->imageRange(Qt::EditRole);

				case Columns::DisplayRange:
					return layer->displayRange(Qt::EditRole);

				default:
					break;
			}

			break;
		}

		case Qt::ToolTipRole:
		{
			switch (index.column()) {
				case Columns::Enabled:
					return layer->flag(Layer::Flags::Enabled, Qt::ToolTipRole);

				case Columns::Type:
					return layer->type(Qt::ToolTipRole);

				case Columns::Locked:
					return layer->flag(Layer::Flags::Frozen, Qt::ToolTipRole);

				case Columns::ID:
					return layer->name(Qt::ToolTipRole);

				case Columns::Name:
					return layer->name(Qt::ToolTipRole);

				case Columns::Frozen:
					return layer->flag(Layer::Flags::Frozen, Qt::ToolTipRole);

				case Columns::Removable:
					return layer->flag(Layer::Flags::Removable, Qt::ToolTipRole);

				case Columns::Mask:
					return layer->flag(Layer::Flags::Mask, Qt::ToolTipRole);

				case Columns::Renamable:
					return layer->flag(Layer::Flags::Renamable, Qt::ToolTipRole);

				case Columns::Order:
					return layer->order(Qt::ToolTipRole);

				case Columns::Opacity:
					return layer->opacity(Qt::ToolTipRole);

				case Columns::WindowNormalized:
					return layer->windowNormalized(Qt::ToolTipRole);

				case Columns::LevelNormalized:
					return layer->levelNormalized(Qt::ToolTipRole);

				case Columns::Color:
					return layer->color(Qt::ToolTipRole);

				case Columns::Image:
					return layer->image(Qt::ToolTipRole);

				case Columns::ImageRange:
					return layer->imageRange(Qt::ToolTipRole);

				case Columns::DisplayRange:
					return layer->displayRange(Qt::ToolTipRole);

				default:
					break;
			}

			break;
		}

		case Qt::TextAlignmentRole:
		{
			switch (index.column()) {
				case Columns::Enabled:
				case Columns::Type:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Columns::Locked:
				case Columns::ID:
				case Columns::Name:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Columns::Frozen:
				case Columns::Removable:
				case Columns::Mask:
				case Columns::Renamable:
				case Columns::Order:
				case Columns::Opacity:
				case Columns::WindowNormalized:
				case Columns::LevelNormalized:
				case Columns::Color:
				case Columns::Image:
					return Qt::AlignRight + Qt::AlignVCenter;

				case Columns::ImageRange:
				case Columns::DisplayRange:
					return Qt::AlignRight + Qt::AlignVCenter;

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QVariant();
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case Columns::Enabled:
			case Columns::Type:
			case Columns::Locked:
				return "";

			case Columns::ID:
				return "ID";

			case Columns::Name:
				return "Name";

			case Columns::Frozen:
				return "Frozen";

			case Columns::Removable:
				return "Removable";

			case Columns::Mask:
				return "Mask";

			case Columns::Renamable:
				return "Renamable";

			case Columns::Order:
				return "Order";

			case Columns::Opacity:
				return "Opacity";

			case Columns::WindowNormalized:
				return "Window";

			case Columns::LevelNormalized:
				return "Level";

			case Columns::Color:
				return "Color";

			case Columns::Image:
				return "Image";

			case Columns::ImageRange:
				return "Image range";

			case Columns::DisplayRange:
				return "Display range";

			default:
				return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags LayersModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	int flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	
	const auto type = data(index.row(), LayersModel::Type, Qt::EditRole).toInt();

	switch (index.column()) {
		case Columns::Enabled:
			flags |= Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
			break;

		case Columns::Type:
		case Columns::Locked:
		case Columns::ID:
			break;

		case Columns::Name:
		{
			if (data(index.row(), LayersModel::Columns::Renamable, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Frozen:
		case Columns::Removable:
			break;

		case Columns::Mask:
		{
			if (type == Layer::Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Order:
			break;

		case Columns::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

		case Columns::WindowNormalized:
		{
			if (type == Layer::Type::Image)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::LevelNormalized:
		{
			if (type == Layer::Type::Image)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Color:
		{
			if (type == Layer::Type::Selection)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Image:
			break;

		case Columns::ImageRange:
		case Columns::DisplayRange:
			break;

		default:
			break;
	}

	return flags;
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (!index.isValid())
		return false;

	int row = index.row();

	auto layer = layers().value(row);

	if (role == Qt::CheckStateRole) {
		switch (index.column()) {
			case Columns::Enabled:
				layer->setFlag(Layer::Flags::Enabled, value == Qt::Checked ? true : false);
				emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
				break;

			default:
				break;
		}
	}

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case Columns::Enabled:
				layer->setFlag(Layer::Flags::Enabled, value.toBool());
				break;

			case Columns::Type:
				layer->setType(static_cast<Layer::Type>(value.toInt()));
				break;

			case Columns::Locked:
				break;

			case Columns::ID:
				layer->setId(value.toString());
				break;

			case Columns::Name:
				layer->setName(value.toString());
				break;

			case Columns::Frozen:
				layer->setFlag(Layer::Flags::Frozen, value.toBool());
				break;

			case Columns::Removable:
				layer->setFlag(Layer::Flags::Removable, value.toBool());
				break;

			case Columns::Mask:
				layer->setFlag(Layer::Flags::Mask, value.toBool());
				break;

			case Columns::Renamable:
				layer->setFlag(Layer::Flags::Renamable, value.toBool());
				break;

			case Columns::Order:
				layer->setOrder(value.toInt());
				break;

			case Columns::Opacity:
				layer->setOpacity(value.toFloat());
				break;

			case Columns::WindowNormalized:
				layer->setWindowNormalized(value.toFloat());
				break;

			case Columns::LevelNormalized:
				layer->setLevelNormalized(value.toFloat());
				break;

			case Columns::Color:
				layer->setColor(value.value<QColor>());
				break;

			case Columns::Image:
				layer->setImage(value.value<QImage>());
				break;

			case Columns::ImageRange:
			case Columns::DisplayRange:
				break;
				
			default:
				break;
		}
	}

	/*
	switch (index.column())
	{
		case Columns::Enabled:
			emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
			break;

		case Columns::Image:
			emit dataChanged(this->index(row, Columns::Image), this->index(row, Columns::DisplayRange));

		case Columns::WindowNormalized:
		case Columns::LevelNormalized:
			emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
			break;

		default:
			emit dataChanged(index, index);
			break;
	}
	*/

	emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));

	return true;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		layers().insert(position, new Layer(this));
	}

	endInsertRows();

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		layers().removeAt(position);
	}

	endRemoveRows();

	return true;
}

bool LayersModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
	beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

	for (int i = 0; i < count; ++i) {
		layers().insert(destinationChild + i, layer(sourceRow));
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		layers().removeAt(removeIndex);
	}
	
	endMoveRows();

	return true;
}

QVariant LayersModel::data(const int& row, const int& column, int role) const
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return QVariant();

	return data(index(row, column), role);
}

void LayersModel::setData(const int& row, const int& column, const QVariant& value)
{
	const auto modelIndex = index(row, column);

	if (!modelIndex.isValid())
		return;

	setData(modelIndex, value);
}

bool LayersModel::mayMoveUp(const int& row)
{
	if (row <= 0)
		return false;

	if (layer(row)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool() || layer(row - 1)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	if (layer(row)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool() || layer(row + 1)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

void LayersModel::moveUp(const int& row)
{
	if (!mayMoveUp(row))
		return;

	moveRows(QModelIndex(), row, 1, QModelIndex(), row - 1);
	sortOrder();
}

void LayersModel::moveDown(const int& row)
{
	if (!mayMoveDown(row))
		return;

	moveRows(QModelIndex(), row + 1, 1, QModelIndex(), row);
	sortOrder();
}

void LayersModel::sortOrder()
{
	for (int row = 0; row < rowCount(); row++)
		setData(row, Columns::Order, row);
}

void LayersModel::removeRows(const QModelIndexList& rows)
{
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (layer(row)->flag(Layer::Flags::Removable, Qt::EditRole).toBool()) {
			rowsToRemove.append(row);
		}
	}

	if (rowsToRemove.isEmpty())
		return;

	beginRemoveRows(QModelIndex(), rowsToRemove.last(), rowsToRemove.first());

	for (auto rowToRemove : rowsToRemove) {
		layers().removeAt(rowToRemove);
	}

	endRemoveRows();

	sortOrder();
}

void LayersModel::renameLayer(const QString& id, const QString& name)
{
	const auto hits = match(index(0, Columns::ID), Qt::EditRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	const auto firstHit = hits.first();

	setData(firstHit.row(), Columns::Name, name);
}

void LayersModel::add(Layer* layer)
{
	insertRows(0, 1);

	setData(index(0, Columns::Enabled, QModelIndex()), layer->flag(Layer::Flags::Enabled, Qt::EditRole));
	setData(index(0, Columns::Type, QModelIndex()), layer->type(Qt::EditRole));
	setData(index(0, Columns::ID, QModelIndex()), layer->id(Qt::EditRole));
	setData(index(0, Columns::Name, QModelIndex()), layer->name(Qt::EditRole));
	setData(index(0, Columns::Frozen, QModelIndex()), layer->flag(Layer::Flags::Frozen, Qt::EditRole));
	setData(index(0, Columns::Removable, QModelIndex()), layer->flag(Layer::Flags::Removable, Qt::EditRole));
	setData(index(0, Columns::Mask, QModelIndex()), layer->flag(Layer::Flags::Mask, Qt::EditRole));
	setData(index(0, Columns::Renamable, QModelIndex()), layer->flag(Layer::Flags::Renamable, Qt::EditRole));
	setData(index(0, Columns::Order, QModelIndex()), layer->order(Qt::EditRole));
	setData(index(0, Columns::Opacity, QModelIndex()), layer->opacity(Qt::EditRole));
	setData(index(0, Columns::WindowNormalized, QModelIndex()), layer->windowNormalized(Qt::EditRole));
	setData(index(0, Columns::LevelNormalized, QModelIndex()), layer->levelNormalized(Qt::EditRole));
	setData(index(0, Columns::Image, QModelIndex()), layer->image(Qt::EditRole));
}

void LayersModel::renameDefaultLayers(const QString& name)
{
	const auto hits = match(index(0, LayersModel::Columns::ID), Qt::DisplayRole, "default", -1, Qt::MatchStartsWith);

	if (hits.isEmpty())
		return;

	for (auto hit : hits) {
		setData(hit.row(), LayersModel::Columns::Name, name);
	}
}

void LayersModel::setDefaultColorImage(const QImage& image)
{
	const auto hits = match(index(0, Columns::ID), Qt::DisplayRole, "default_color", -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	setData(hits.first().row(), Columns::Image, image);
}

void LayersModel::setDefaultSelectionImage(const QImage& image)
{
	const auto hits = match(index(0, Columns::ID), Qt::DisplayRole, "default_selection", -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return;

	setData(hits.first().row(), Columns::Image, image);
}

Layers& LayersModel::layers()
{
	return _imageDataset->layers();
}

const Layers& LayersModel::layers() const
{
	return _imageDataset->layers();
}

Layer* LayersModel::layer(const int& id)
{
	return _imageDataset->layers()[id];
}