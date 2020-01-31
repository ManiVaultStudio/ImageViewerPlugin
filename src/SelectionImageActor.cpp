#include "SelectionImageActor.h"

#include <QDebug>

#include "Renderer.h"

#include "SelectionImageQuad.h"

SelectionImageActor::SelectionImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	_opacity = 0.5f;

	addShape<SelectionImageQuad>("Quad");
}

void SelectionImageActor::setImage(std::shared_ptr<QImage> image)
{
	shape<SelectionImageQuad>("Quad")->setImage(image);
}

QSize SelectionImageActor::imageSize() const
{
	return dynamic_cast<SelectionImageQuad*>(_shapes["Quad"].get())->size();
}