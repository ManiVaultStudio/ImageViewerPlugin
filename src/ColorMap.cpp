#include "ColorMap.h"

#include <QDebug>

ColorMap::ColorMap(const QString& name /*= ""*/, const QString& resourcePath /*= ""*/, const Type& type /*= Type::OneDimensional*/, const QImage& image /*= QImage()*/) :
	_name(name),
	_resourcePath(resourcePath),
	_type(type),
	_image(image)
{

}

QString ColorMap::name() const
{
	return _name;
}

QString ColorMap::resourcePath() const
{
	return _resourcePath;
}

ColorMap::Type ColorMap::type() const
{
	return _type;
}

QImage ColorMap::image() const
{
	return _image;
}

int ColorMap::noDimensions() const
{
	return ult(_type);
}
