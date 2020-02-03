#include "SelectionImageActor.h"

#include <QDebug>

#include "Renderer.h"

#include "SelectionImage.h"

SelectionImageActor::SelectionImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	_opacity = 0.5f;

	addProp<SelectionImage>("Quad");
}

void SelectionImageActor::setImage(std::shared_ptr<QImage> image)
{
	prop<SelectionImage>("Quad")->setImage(image);
}

QSize SelectionImageActor::imageSize() const
{
	return dynamic_cast<SelectionImage*>(_props["Quad"].get())->imageSize();
}