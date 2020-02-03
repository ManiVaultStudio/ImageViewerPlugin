#include "SelectionImageActor.h"
#include "SelectionImageProp.h"
#include "Renderer.h"

#include <QDebug>

SelectionImageActor::SelectionImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
	_opacity = 0.5f;

	addProp<SelectionImageProp>("Quad");
}

void SelectionImageActor::setImage(std::shared_ptr<QImage> image)
{
	prop<SelectionImageProp>("Quad")->setImage(image);
}

QSize SelectionImageActor::imageSize() const
{
	return dynamic_cast<SelectionImageProp*>(_props["Quad"].get())->imageSize();
}