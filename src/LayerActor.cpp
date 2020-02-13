#include "LayerActor.h"
#include "Renderer.h"

#include <QOpenGLTexture>
#include <QDebug>

LayerActor::LayerActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_layer(nullptr)
{
	/*
	addProp<ColorImageProp>("ColorImageProp");

	connect(propByName<ColorImageProp>("ColorImageProp"), &ColorImageProp::imageSizeChanged, this, &LayerActor::imageSizeChanged);
	*/
}

void LayerActor::setLayer(Layer* layer)
{
	if (layer == _layer)
		return;

	if (_layer != nullptr) {
		disconnect(_layer, &Layer::imageChanged, this, &LayerActor::onImageChanged);
	}

	_layer = layer;

	qDebug() << name() << "set layer";

	connect(_layer, &Layer::imageChanged, this, &LayerActor::onImageChanged);
}

void LayerActor::onImageChanged(const QImage& image)
{
	qDebug() << name() << "on image changed";
}