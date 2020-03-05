#include "LayersModel.h"
#include "DatasetsModel.h"

#include <QItemSelectionModel>
#include <QFont>
#include <QBrush>
#include <QDebug>

LayersModel::LayersModel(Layers* layers) :
	QAbstractListModel(),
	_layers(layers)
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return _layers->size();
}

int LayersModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
	Q_UNUSED(parent);

	return 11;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _layers->size() || index.row() < 0)
		return QVariant();

	auto layer = _layers->at(index.row());

	switch (role)
	{
		case Qt::FontRole:
		{
			if (index.column() == Columns::Locked)
				return QFont("Font Awesome 5 Free Solid", 6, 1);

			break;
		}

		case Qt::ForegroundRole:
			if (index.column() == Columns::Locked)
				return QBrush(Qt::black);
			else
				return layer->isFlagSet(Layer::Flags::Enabled) ? QBrush(Qt::black) : QBrush(Qt::darkGray);

		case Qt::DisplayRole:
		{
			switch (index.column()) {
				case Columns::Locked:
					return layer->isFlagSet(Layer::Flags::Fixed) ? u8"\uf023" : u8"\uf09c";

				case Columns::Name:
					return layer->_name;

				case Columns::Type:
				{
					switch (layer->_type)
					{
						case Layer::Type::Image:
							return "Image";

						case Layer::Type::Selection:
							return "Selection";

						case Layer::Type::Metadata:
							return "Metadata";

						default:
							break;
					}
				}

				case Columns::Enabled:
					return layer->isFlagSet(Layer::Flags::Enabled) ? "true" : "false";

				case Columns::Fixed:
					return layer->isFlagSet(Layer::Flags::Fixed) ? "true" : "false";

				case Columns::Removable:
					return layer->isFlagSet(Layer::Flags::Removable) ? "true" : "false";

				case Columns::Order:
					return QString::number(layer->_order);

				case Columns::Opacity:
					return QString("%1%").arg(QString::number(100.0f * layer->_opacity, 'f', 1));

				case Columns::Window:
					return QString::number(layer->_window, 'f', 2);

				case Columns::Level:
					return QString::number(layer->_level, 'f', 2);

				case Columns::Color:
					return layer->_color.name();

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
		{
			switch (index.column()) {
				case Columns::Locked:
					return layer->isFlagSet(Layer::Flags::Fixed);

				case Columns::Name:
					return layer->_name;

				case Columns::Type:
					return static_cast<int>(layer->_type);

				case Columns::Enabled:
					return layer->isFlagSet(Layer::Flags::Enabled);

				case Columns::Fixed:
					return layer->isFlagSet(Layer::Flags::Fixed);

				case Columns::Removable:
					return layer->isFlagSet(Layer::Flags::Removable);

				case Columns::Order:
					return layer->_order;

				case Columns::Opacity:
					return layer->_opacity;

				case Columns::Window:
					return layer->_window;

				case Columns::Level:
					return layer->_level;

				case Columns::Color:
					return layer->_color;

				default:
					break;
			}

			break;
		}

		case Qt::TextAlignmentRole:
		{
			switch (index.column()) {
				case Columns::Locked:
				case Columns::Name:
				case Columns::Type:
					return Qt::AlignLeft + Qt::AlignVCenter;

				case Columns::Enabled:
				case Columns::Fixed:
				case Columns::Order:
				case Columns::Opacity:
				case Columns::Window:
				case Columns::Level:
				case Columns::Color:
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
			case Columns::Locked:
				return "";

			case Columns::Name:
				return "Name";

			case Columns::Type:
				return "Type";

			case Columns::Enabled:
				return "Enabled";

			case Columns::Fixed:
				return "Fixed";

			case Columns::Removable:
				return "Removable";

			case Columns::Order:
				return "Order";

			case Columns::Opacity:
				return "Opacity";

			case Columns::Window:
				return "Window";

			case Columns::Level:
				return "Level";

			case Columns::Color:
				return "Color";

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
		case Columns::Locked:
			break;

		case Columns::Name:
		{
			if (!data(index.row(), LayersModel::Columns::Fixed, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Type:
		case Columns::Fixed:
		case Columns::Removable:
			break;

		case Columns::Enabled:
			flags |= Qt::ItemIsEditable;
			break;

		case Columns::Order:
			break;

		case Columns::Opacity:
			flags |= Qt::ItemIsEditable;
			break;

		case Columns::Window:
		{
			if (type == Layer::Type::Image)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Level:
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

		default:
			break;
	}

	return flags;
}

bool LayersModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::DisplayRole*/)
{
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto layer = _layers->value(row);

		switch (index.column()) {
			case Columns::Locked:
				break;

			case Columns::Name:
				layer->_name = QString("%1").arg(value.toString());
				break;
			
			case Columns::Type:
				layer->_type = static_cast<Layer::Type>(value.toInt());
				break;

			case Columns::Enabled:
				layer->setFlag(Layer::Flags::Enabled, value.toBool());
				break;

			case Columns::Fixed:
				layer->setFlag(Layer::Flags::Fixed, value.toBool());
				break;

			case Columns::Removable:
				layer->setFlag(Layer::Flags::Removable, value.toBool());
				break;

			case Columns::Order:
				layer->_order = value.toInt();
				break;

			case Columns::Opacity:
				layer->_opacity = value.toFloat();
				break;

			case Columns::Window:
				layer->_window = value.toFloat();
				break;

			case Columns::Level:
				layer->_level = value.toFloat();
				break;

			case Columns::Color:
				layer->_color = value.value<QColor>();
				break;

			default:
				return false;
		}

		_layers->replace(row, layer);

		if (index.column() == Columns::Enabled) {
			emit dataChanged(this->index(row, 0), this->index(row, rowCount() - 1));
		}
		else {
			emit dataChanged(index, index);
		}

		return true;
	}

	return false;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_layers->insert(position, new Layer(this));
	}

	endInsertRows();

	return true;
}

bool LayersModel::removeRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginRemoveRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; ++row) {
		_layers->removeAt(position);
	}

	endRemoveRows();

	return true;
}

QVariant LayersModel::data(const int& row, const int& column, int role /*= Qt::DisplayRole*/) const
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

	auto layers = (*_layers);

	if (layers[row]->isFlagSet(Layer::Flags::Fixed) || layers[row - 1]->isFlagSet(Layer::Flags::Fixed))
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	auto layers = (*_layers);

	if (layers[row]->isFlagSet(Layer::Flags::Fixed) || layers[row + 1]->isFlagSet(Layer::Flags::Fixed))
		return false;

	return true;
}

void LayersModel::moveUp(const int& row)
{
	if (!mayMoveUp(row))
		return;

	if (row > 0 && row < _layers->count())
	{
		beginMoveRows(QModelIndex(), row, row, QModelIndex(), row - 1);

		auto layers = (*_layers);

		std::swap(layers[row]->_order, layers[layers[row]->_order - 1]->_order);

		std::sort(_layers->begin(), _layers->end(), [](Layer* layerA, Layer* layerB) {
			return layerA->_order < layerB->_order;
		});
		
		endMoveRows();
	}
}

void LayersModel::moveDown(const int& row)
{
	if (!mayMoveDown(row))
		return;

	if (row >= 0 && row < _layers->count() - 1)
	{
		moveUp(row + 1);
	}
}

void LayersModel::removeRows(const QModelIndexList& rows)
{
	QList<int> rowsToRemove;

	for (const auto& index : rows) {
		const auto row = index.row();

		if (_layers->at(row)->isFlagSet(Layer::Flags::Removable)) {
			rowsToRemove.append(row);
		}
	}

	if (rowsToRemove.isEmpty())
		return;

	std::sort(rowsToRemove.begin(), rowsToRemove.end());
	std::reverse(rowsToRemove.begin(), rowsToRemove.end());

	beginRemoveRows(QModelIndex(), rowsToRemove.last(), rowsToRemove.first());

	for (auto rowToRemove : rowsToRemove) {
		_layers->removeAt(rowToRemove);
	}

	endRemoveRows();
}