#include "SelectionImageActor.h"

#include <QDebug>

#include "Renderer.h"

#include "SelectionImageQuad.h"

SelectionImageActor::SelectionImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	_receiveMouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);

	addShape<SelectionImageQuad>("Quad");
}

bool SelectionImageActor::isInitialized() const
{
	return true;
}

void SelectionImageActor::setImage(std::shared_ptr<QImage> image)
{
	shape<SelectionImageQuad>("Quad")->setImage(image);
}

QSize SelectionImageActor::imageSize() const
{
	return dynamic_cast<SelectionImageQuad*>(_shapes["Quad"].get())->size();
}