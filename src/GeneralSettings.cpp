#include "GeneralSettings.h"

#include <QFont>
#include <QDebug>

GeneralSettings::GeneralSettings(QObject* parent, Dataset* dataset, const QString& id, const QString& name, const Layer::Type& type, const std::uint32_t& flags) :
	Settings(parent, dataset),
	_id(id),
	_name(name),
	_type(type),
	_flags(flags),
	_order(0),
	_opacity(0.0f),
	_colorMap(),
	_image(),
	_imageRange(),
	_displayRange(),
	_windowNormalized(1.0f),
	_levelNormalized(0.5f),
	_window(1.0f),
	_level(0.5f)
{
}

Qt::ItemFlags GeneralSettings::itemFlags(const Layer::Column& column) const
{
	
}

QVariant GeneralSettings::data(const Layer::Column& column, int role) const
{
	
}

void GeneralSettings::setData(const Layer::Column& column, const QVariant& value, const int& role)
{
	
}

