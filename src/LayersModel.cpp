#include "LayersModel.h"
#include "ImageViewerPlugin.h"

#include <QItemSelectionModel>
#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(ImageViewerPlugin* imageViewerPlugin) :
	QAbstractListModel(imageViewerPlugin),
	_imageViewerPlugin(imageViewerPlugin),
	_layers(),
	_selectionModel()
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return _layers.size();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 19;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= rowCount() || index.row() < 0)
		return QVariant();

	auto layer = _layers.at(index.row());

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
					return layer->id(Qt::DisplayRole);

				case Columns::Name:
					return layer->name(Qt::DisplayRole);

				case Columns::Dataset:
					return layer->dataset(Qt::DisplayRole);

				case Columns::Flags:
					return layer->flags(Qt::DisplayRole);

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

				case Columns::ColorMap:
					return layer->colorMap(Qt::DisplayRole);

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

				case Columns::Dataset:
					return layer->dataset(Qt::EditRole);

				case Columns::Flags:
					return layer->flags(Qt::EditRole);

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

				case Columns::ColorMap:
					return layer->colorMap(Qt::EditRole);

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

				case Columns::Dataset:
					return layer->dataset(Qt::ToolTipRole);

				case Columns::Flags:
					return layer->flags(Qt::ToolTipRole);

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

				case Columns::ColorMap:
					return layer->colorMap(Qt::ToolTipRole);

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
				case Columns::Dataset:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Columns::Flags:
				case Columns::Frozen:
				case Columns::Removable:
				case Columns::Mask:
				case Columns::Renamable:
				case Columns::Order:
				case Columns::Opacity:
				case Columns::WindowNormalized:
				case Columns::LevelNormalized:
				case Columns::ColorMap:
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

			case Columns::Dataset:
				return "Dataset";

			case Columns::Flags:
				return "Flags";

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

			case Columns::ColorMap:
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

		case Columns::Dataset:
		case Columns::Flags:
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

		case Columns::ColorMap:
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

	auto layer = _layers.value(row);

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

			case Columns::Dataset:
				break;

			case Columns::Flags:
				layer->setFlags(value.toInt());
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

			case Columns::ColorMap:
				layer->setColorMap(value.value<QImage>());
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
	/*
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_layers.insert(position, new Layer(_imageViewerPlugin));
	}

	endInsertRows();
	*/

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_layers.removeAt(position);
	}

	endRemoveRows();

	return true;
}

bool LayersModel::moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild)
{
	beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(), destinationChild);

	for (int i = 0; i < count; ++i) {
		_layers.insert(destinationChild + i, _layers.at(sourceRow));
		int removeIndex = destinationChild > sourceRow ? sourceRow : sourceRow + 1;
		_layers.removeAt(removeIndex);
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

	if (_layers.at(row)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool() || _layers.at(row - 1)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool())
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	if (_layers.at(row)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool() || _layers.at(row + 1)->flag(Layer::Flags::Frozen, Qt::EditRole).toBool())
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
		setData(row, Columns::Order, rowCount() - row);
}

void LayersModel::removeRows(const QModelIndexList& rows)
{
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (_layers.at(row)->flag(Layer::Flags::Removable, Qt::EditRole).toBool()) {
			rowsToRemove.append(row);
		}
	}

	if (rowsToRemove.isEmpty())
		return;

	std::sort(rowsToRemove.begin(), rowsToRemove.end(), std::greater<int>());

	beginRemoveRows(QModelIndex(), rowsToRemove.last(), rowsToRemove.first());

	for (auto rowToRemove : rowsToRemove) {
		_layers.removeAt(rowToRemove);
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

Layer* LayersModel::findLayerById(const QString& id)
{
	const auto hits = match(index(0, LayersModel::Columns::ID), Qt::DisplayRole, id, -1, Qt::MatchExactly);

	if (hits.isEmpty())
		return nullptr;

	return _layers[hits.first().row()];
}

void LayersModel::addLayer(Layer* layer)
{
	beginInsertRows(QModelIndex(), 0, 0);

	_layers.insert(0, layer);

	endInsertRows();

	sortOrder();
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