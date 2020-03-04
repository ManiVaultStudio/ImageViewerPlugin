#include "Layer.h"

#include <QDebug>

Layer::Layer(QObject* parent) :
	QObject(parent),
	_name(""),
	_type(Type::Image),
	_flags(0),
	_order(0),
	_opacity(1.0f),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(1.0f)
{
}

Layer::Layer(QObject* parent, const QString& name, const Type& type, const std::uint32_t& flags, const std::uint32_t& order, const float& opacity /*= 1.0f*/, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_name(name),
	_type(type),
	_flags(flags),
	_order(order),
	_opacity(opacity),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(window),
	_level(level)
{
}

bool Layer::isFlagSet(const std::uint32_t& flag) const
{
	return _flags & flag;
}

void Layer::setFlag(const std::uint32_t& flag, const bool& enabled /*= true*/)
{
	if (enabled)
		_flags |= flag;
	else
		_flags = _flags & ~flag;
}