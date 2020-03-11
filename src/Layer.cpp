#include "Layer.h"

#include <QDebug>

Layer::Layer(QObject* parent) :
	QObject(parent),
	_id(),
	_name(),
	_type(Type::Image),
	_flags(0),
	_order(0),
	_opacity(1.0f),
	_image(this),
	_color()
{
}

Layer::Layer(QObject* parent, const QString& id, const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity /*= 1.0f*/, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(order),
	_opacity(opacity),
	_image(this),
	_color()
{
}

QVariant Layer::id(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _id;

		case Qt::ToolTipRole:
			return QString("ID: %1").arg(_id);

		default:
			break;
	}

	return QString();
}

void Layer::setId(const QString& id)
{
	_id = id;
}

QVariant Layer::name(const int& role /*= Qt::DisplayRole*/) const
{
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
			return _name;

		case Qt::ToolTipRole:
			return QString("Name: %1").arg(_name);

		default:
			break;
	}

	return QString();
}

void Layer::setName(const QString& name)
{
	_name = name;
}

QVariant Layer::type(const int& role /*= Qt::DisplayRole*/) const
{
	const auto typeName = Layer::typeName(_type);

	switch (role)
	{
		case Qt::DisplayRole:
		{
			switch (_type) {
				case Layer::Type::Image:
					return u8"\uf03e";

				case Layer::Type::Selection:
					return u8"\uf065";

				case Layer::Type::Cluster:
					return u8"\uf02c";

				default:
					break;
			}

			break;
		}

		case Qt::EditRole:
			return static_cast<int>(_type);

		case Qt::ToolTipRole:
			return QString("Type: %1").arg(typeName);

		default:
			break;
	}

	return QString();
}

void Layer::setType(const Type& type)
{
	_type = type;
}

QVariant Layer::flag(const std::uint32_t& flag, const int& role /*= Qt::DisplayRole*/) const
{
	const auto isFlagSet	= _flags & flag;
	const auto flagString	= isFlagSet ? "true" : "false";

	switch (role)
	{
		case Qt::DisplayRole:
			return flagString;

		case Qt::EditRole:
			return isFlagSet;

		case Qt::ToolTipRole:
		{
			switch (flag)
			{
				case Enabled:
					return QString("Enabled: %1").arg(flagString);

				case Frozen:
					return QString("Frozen: %1").arg(flagString);

				case Removable:
					return QString("Removable: %1").arg(flagString);

				case Mask:
					return QString("Mask: %1").arg(flagString);

				case Renamable:
					return QString("Renamable: %1").arg(flagString);

				default:
					break;
			}

			break;
		}

		default:
			break;
	}

	return QString();
}

void Layer::setFlag(const std::uint32_t& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= flag;
	else
		_flags = _flags & ~flag;
}

QVariant Layer::order(const int& role /*= Qt::DisplayRole*/) const
{
	const auto orderString = QString::number(_order);

	switch (role)
	{
		case Qt::DisplayRole:
			return orderString;

		case Qt::EditRole:
			return _order;

		case Qt::ToolTipRole:
			return QString("Order: %1").arg(orderString);

		default:
			break;
	}

	return QString();
}

void Layer::setOrder(const std::uint32_t& order)
{
	_order = order;
}
