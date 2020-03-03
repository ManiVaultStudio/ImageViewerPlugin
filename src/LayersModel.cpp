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

	return 9;
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
		/*
		case Qt::FontRole:
		{
			QFont font;

			if (layer->_fixed)
				font.setBold(true);

			return font;
		}
		*/

		case Qt::ForegroundRole:
			return layer->_enabled ? QBrush(Qt::black) : QBrush(Qt::darkGray);

		case Qt::DisplayRole:
		{
			switch (index.column()) {
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
					return layer->_enabled;

				case Columns::Fixed:
					return layer->_fixed ? "true" : "false";

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
				case Columns::Name:
					return layer->_name;

				case Columns::Type:
					return static_cast<int>(layer->_type);

				case Columns::Enabled:
					return layer->_enabled;

				case Columns::Fixed:
					return layer->_fixed;

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
			case Columns::Name:
				return "Name";

			case Columns::Type:
				return "Type";

			case Columns::Enabled:
				return "Enabled";

			case Columns::Fixed:
				return "Fixed";

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

	switch (index.column()) {
		case Columns::Name:
		case Columns::Type:
		case Columns::Fixed:
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
			if (type(index.row(), Qt::EditRole).toInt() == Layer::Type::Image)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Level:
		{
			if (type(index.row(), Qt::EditRole).toInt() == Layer::Type::Image)
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Color:
		{
			if (type(index.row(), Qt::EditRole).toInt() == Layer::Type::Selection)
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
			case Columns::Name:
				layer->_name = QString("*%1").arg(value.toString());
				break;
			
			case Columns::Type:
				layer->_type = static_cast<Layer::Type>(value.toInt());
				break;

			case Columns::Enabled:
				layer->_enabled = value.toBool();
				break;

			case Columns::Fixed:
				layer->_fixed = value.toBool();
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

QVariant LayersModel::name(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Name), role);
}

QVariant LayersModel::type(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Type), role);
}

QVariant LayersModel::enabled(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Enabled), role);
}

QVariant LayersModel::fixed(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Fixed), role);
}

QVariant LayersModel::order(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Order), role);
}

QVariant LayersModel::opacity(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Opacity), role);
}

QVariant LayersModel::window(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Window), role);
}

QVariant LayersModel::level(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Level), role);
}

QVariant LayersModel::color(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, Columns::Color), role);
}

void LayersModel::setName(const int& row, const QString& name)
{
	const auto modelIndex = index(row, Columns::Name);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setType(const int& row, const int& type)
{
	const auto modelIndex = index(row, Columns::Type);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setEnabled(const int& row, const bool& enabled)
{
	const auto modelIndex = index(row, Columns::Enabled);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setOrder(const int& row, const int& order)
{
	const auto modelIndex = index(row, Columns::Order);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setOpacity(const int& row, const float& opacity)
{
	const auto modelIndex = index(row, Columns::Opacity);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setWindow(const int& row, const float& window)
{
	const auto modelIndex = index(row, Columns::Window);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setLevel(const int& row, const float& level)
{
	const auto modelIndex = index(row, Columns::Level);

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setColor(const int& row, const QColor& color)
{
	const auto modelIndex = index(row, Columns::Color);

	if (!modelIndex.isValid())
		return;
}

bool LayersModel::mayMoveUp(const int& row)
{
	if (row <= 0)
		return false;

	auto layers = (*_layers);

	if (layers[row]->_fixed || layers[row - 1]->_fixed)
		return false;

	return true;
}

bool LayersModel::mayMoveDown(const int& row)
{
	if (row >= rowCount() - 1)
		return false;

	auto layers = (*_layers);

	if (layers[row]->_fixed || layers[row + 1]->_fixed)
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