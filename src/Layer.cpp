#include "Layer.h"

#include <QDebug>

Layer::Layer(QObject* parent) :
	QObject(parent),
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

Layer::Layer(QObject* parent, const QString& name, const Type& type, const bool& enabled, const bool& fixed, const std::uint32_t& order, const float& opacity /*= 1.0f*/, const float& window /*= 1.0f*/, const float& level /*= 0.5f*/) :
	QObject(parent),
	_name(name),
	_type(type),
	_enabled(enabled),
	_fixed(fixed),
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