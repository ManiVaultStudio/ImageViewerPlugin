#include "SelectionPickerActor.h"

#include <QDebug>

#include "Renderer.h"

#include "SelectionImageQuad.h"

SelectionPickerActor::SelectionPickerActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_imageSize()
{
	_registeredEvents = static_cast<int>(ActorEvent::MousePress) | static_cast<int>(ActorEvent::MouseRelease) | static_cast<int>(ActorEvent::MouseMove);

//	addShape<SelectionImageQuad>("Quad");
}

QSize SelectionPickerActor::imageSize() const
{
	return _imageSize;
}

void SelectionPickerActor::setImageSize(const QSize& imageSize)
{
	if (imageSize == _imageSize)
		return;

	qDebug() << "Set image size";

	_imageSize = imageSize;

	emit imageSizeChanged(_imageSize);
}