#include "LayersModel.h"
#include "DatasetsModel.h"

#include <QItemSelectionModel>
#include <QDebug>

LayersModel::LayersModel(Layers* layers) :
	QAbstractListModel(),
	_layers(layers),
	_selectionModel(new QItemSelectionModel(this))
{
}

LayersModel::~LayersModel() = default;

int LayersModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return _layers->size();
}

int LayersModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	return 6;
}

QVariant LayersModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _layers->size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole) {
		auto layer = _layers->at(index.row());

		switch (index.column()) {
			case (static_cast<int>(Columns::Name)) :
				return layer._name;

			case (static_cast<int>(Columns::Type)):
			{
				switch (layer._type)
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

			case (static_cast<int>(Columns::Enabled)):
				return layer._enabled;

			case (static_cast<int>(Columns::Order)):
				return layer._order;

			case (static_cast<int>(Columns::Opacity)):
				return QString::number(layer._opacity, 'f', 1);

			case (static_cast<int>(Columns::Window)):
				return QString::number(layer._window, 'f', 1);

			case (static_cast<int>(Columns::Level)):
				return QString::number(layer._level, 'f', 1);

			default:
				break;
		}
	}

	return QVariant();
}

QVariant LayersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal) {
		switch (section) {
			case (static_cast<int>(Columns::Name)):
				return "Name";

			case (static_cast<int>(Columns::Type)):
				return "Type";

			case (static_cast<int>(Columns::Enabled)):
				return "Enabled";

			case (static_cast<int>(Columns::Order)):
				return "Type";

			case (static_cast<int>(Columns::Opacity)):
				return "Opacity";

			case (static_cast<int>(Columns::Window)):
				return "Window";

			case (static_cast<int>(Columns::Level)):
				return "Level";

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

	int flags = Qt::ItemIsSelectable;

	/*
	if (enabled(index.row()))
		flags |= Qt::ItemIsEnabled;
	*/

	flags |= Qt::ItemIsEnabled;

	switch (index.column()) {
		case (static_cast<int>(Columns::Name)):
			break;

		case (static_cast<int>(Columns::Type)):
			break;

		case (static_cast<int>(Columns::Enabled)):
			break;

		case (static_cast<int>(Columns::Order)):
			break;

		case (static_cast<int>(Columns::Opacity)):
			flags |= Qt::ItemIsEditable;
			break;

		case (static_cast<int>(Columns::Window)):
		case (static_cast<int>(Columns::Level)):
		{
			if (type(index.row()) == static_cast<int>(Layer::Type::Image))
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
			case (static_cast<int>(Columns::Name)):
				layer._name = value.toString();
				break;
			
			case (static_cast<int>(Columns::Enabled)):
				layer._enabled = value.toBool();
				break;

			case (static_cast<int>(Columns::Order)):
				layer._order = value.toInt();
				break;

			case (static_cast<int>(Columns::Opacity)):
				layer._opacity = value.toFloat();
				break;

			case (static_cast<int>(Columns::Window)):
				layer._window = value.toFloat();
				break;

			case (static_cast<int>(Columns::Level)):
				layer._level = value.toFloat();
				break;

			default:
				return false;
		}

		_layers->replace(row, layer);

		emit(dataChanged(index, index));

		return true;
	}

	return false;
}

bool LayersModel::insertRows(int position, int rows, const QModelIndex& index /*= QModelIndex()*/)
{
	Q_UNUSED(index);

	beginInsertRows(QModelIndex(), position, position + rows - 1);

	for (int row = 0; row < rows; row++) {
		_layers->insert(position, Layer());
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

QString LayersModel::name(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Name)), role).toString();
}

int LayersModel::type(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Type)), role).toInt();
}

bool LayersModel::enabled(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Enabled)), role).toBool();
}

int LayersModel::order(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Order)), role).toInt();
}

float LayersModel::opacity(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Opacity)), role).toFloat();
}

float LayersModel::window(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Window)), role).toFloat();
}

float LayersModel::level(const int& row, int role /*= Qt::DisplayRole*/) const
{
	return data(index(row, static_cast<int>(LayersModel::Columns::Level)), role).toFloat();
}

void LayersModel::setName(const int& row, const QString& name)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Name));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setType(const int& row, const int& type)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Type));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setEnabled(const int& row, const bool& enabled)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Enabled));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setOrder(const int& row, const int& order)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Order));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setOpacity(const int& row, const float& opacity)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Opacity));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setWindow(const int& row, const float& window)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Window));

	if (!modelIndex.isValid())
		return;


}

void LayersModel::setLevel(const int& row, const float& level)
{
	const auto modelIndex = index(row, static_cast<int>(LayersModel::Columns::Level));

	if (!modelIndex.isValid())
		return;


}