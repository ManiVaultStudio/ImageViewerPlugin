#include "SelectionOverlayActor.h"

#include <QDebug>

#include "Renderer.h"

#include "ImageQuad.h"

SelectionOverlayActor::SelectionOverlayActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	_receiveMouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);

	addShape<ImageQuad>("Quad");
}

bool SelectionOverlayActor::isInitialized() const
{
	return true;
}

void SelectionOverlayActor::setImage(std::shared_ptr<QImage> image)
{
	shape<ImageQuad>("Quad")->setImage(image);
}

QSize SelectionOverlayActor::imageSize() const
{
	return dynamic_cast<ImageQuad*>(_shapes["Quad"].get())->size();
}