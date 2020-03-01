#include "Layer.h"

#include <QDebug>

Layer::Layer() :
	_name(""),
	_type(Type::Image),
	_enabled(true),
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

Layer::Layer(const QString& name, const Type& type, const bool& enabled, const std::uint32_t& order, const float& opacity /*= 1.0f*/) :
	_name(name),
	_type(type),
	_enabled(enabled),
	_order(order),
	_opacity(opacity),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(1.0f)
{
}