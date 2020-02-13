#include "Layer.h"
#include "Dataset.h"

#include <QDebug>

Layer::Layer(Dataset* dataset) :
	QObject(),
	_dataset(dataset),
	_name(),
	_order(0),
	_opacity(),
	_image()
{
}

Layer::~Layer() = default;

Dataset* Layer::dataset()
{
	return _dataset;
}

QString Layer::name() const
{
	return _name;
}

void Layer::setName(const QString& name)
{
	if (name == _name)
		return;

	const auto oldName = fullName();

	_name = name;

	qDebug() << oldName << "rename to" << fullName();

	emit nameChanged(_name);
}

QString Layer::fullName() const
{
	return QString("%1::%2").arg(_dataset->name(), _name);
}

std::uint32_t Layer::order() const
{
	return _order;
}

void Layer::setOrder(const std::uint32_t& order)
{
	if (order == _order)
		return;

	_order = order;

	qDebug() << fullName() << "set order to" << _order;

	emit orderChanged(_order);
}

float Layer::opacity() const
{
	return _opacity;
}

void Layer::setOpacity(const float& opacity)
{
	if (opacity == _opacity)
		return;

	_opacity = opacity;

	qDebug() << fullName() << "set opacity to" << _opacity;

	emit opacityChanged(_opacity);
}

const QImage Layer::image() const
{
	return _image;
}

void Layer::setImage(const QImage& image)
{
	if (image == _image)
		return;

	_image = image;

	qDebug() << fullName() << "set image";

	emit opacityChanged(_opacity);
}