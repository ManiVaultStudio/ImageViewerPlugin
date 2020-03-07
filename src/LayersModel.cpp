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
	QObject::connect(this, &LayersModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles /*= QVector<int>()*/) {
		if (topLeft.column() <= LayersModel::Columns::Image && bottomRight.column() >= LayersModel::Columns::Image) {
			
		}
	});
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

	return 14;
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
					return layer->name();

				case Columns::Type:
				{
					switch (layer->type())
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
					return QString::number(layer->order());

				case Columns::Opacity:
					return QString("%1%").arg(QString::number(100.0f * layer->opacity(), 'f', 1));

				case Columns::WindowNormalized:
					return QString::number(layer->image().windowNormalized(), 'f', 2);

				case Columns::LevelNormalized:
					return QString::number(layer->image().levelNormalized(), 'f', 2);

				case Columns::Color:
					return layer->color().name();

				case Columns::Image:
					return "Image";

				case Columns::ImageRange:
					return QString("[%1, %2]").arg(QString::number(layer->image().imageRange().min(), 'f', 2), QString::number(layer->image().imageRange().max(), 'f', 2));

				case Columns::DisplayRange:
					return QString("[%1, %2]").arg(QString::number(layer->image().displayRange().min(), 'f', 2), QString::number(layer->image().displayRange().max(), 'f', 2));

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
					return layer->name();

				case Columns::Type:
					return static_cast<int>(layer->type());

				case Columns::Enabled:
					return layer->isFlagSet(Layer::Flags::Enabled);

				case Columns::Fixed:
					return layer->isFlagSet(Layer::Flags::Fixed);

				case Columns::Removable:
					return layer->isFlagSet(Layer::Flags::Removable);

				case Columns::Order:
					return layer->order();

				case Columns::Opacity:
					return layer->opacity();

				case Columns::WindowNormalized:
					return layer->image().windowNormalized();

				case Columns::LevelNormalized:
					return layer->image().levelNormalized();

				case Columns::Color:
					return layer->color();

					/*
				case Columns::Image:
					return layer->image();
					*/

				case Columns::ImageRange:
					return QVariant::fromValue(layer->image().imageRange());

				case Columns::DisplayRange:
					return QVariant::fromValue(layer->image().displayRange());

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
				case Columns::Removable:
				case Columns::Order:
				case Columns::Opacity:
				case Columns::WindowNormalized:
				case Columns::LevelNormalized:
				case Columns::Color:
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
		case Columns::Locked:
			break;

		case Columns::Name:
		{
			if (!data(index.row(), LayersModel::Columns::Fixed, Qt::EditRole).toBool())
				flags |= Qt::ItemIsEditable;

			break;
		}

		case Columns::Type:
			break;

		case Columns::Enabled:
			flags |= Qt::ItemIsEditable;
			break;

		case Columns::Fixed:
		case Columns::Removable:
			break;

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
	if (index.isValid() && role == Qt::DisplayRole) {
		int row = index.row();

		auto layer = _layers->value(row);

		switch (index.column()) {
			case Columns::Locked:
				break;

			case Columns::Name:
				layer->setName(QString("%1").arg(value.toString()));
				break;
			
			case Columns::Type:
				layer->setType(static_cast<Layer::Type>(value.toInt()));
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
				layer->setOrder(value.toInt());
				break;

			case Columns::Opacity:
				layer->setOpacity(value.toFloat());
				break;

			case Columns::WindowNormalized:
				layer->image().setWindowNormalized(value.toFloat());
				break;

			case Columns::LevelNormalized:
				layer->image().setLevelNormalized(value.toFloat());
				break;

			case Columns::Color:
				layer->setColor(value.value<QColor>());
				break;

			case Columns::Image:
				layer->image().setImage(value.value<QImage>());
				break;

			case Columns::ImageRange:
			case Columns::DisplayRange:
				break;
				
			default:
				return false;
		}

		_layers->replace(row, layer);

		switch (index.column())
		{
			case Columns::Enabled:
				emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
				break;

			case Columns::Image:
			case Columns::WindowNormalized:
			case Columns::LevelNormalized:
				emit dataChanged(this->index(row, 0), this->index(row, columnCount() - 1));
				break;

			default:
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

		std::swap(layers[row]->order(), layers[layers[row]->order() - 1]->order());

		std::sort(_layers->begin(), _layers->end(), [](Layer* layerA, Layer* layerB) {
			return layerA->order() < layerB->order();
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