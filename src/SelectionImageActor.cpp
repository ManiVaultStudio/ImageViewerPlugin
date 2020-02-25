#include "SelectionImageActor.h"
#include "SelectionImageProp.h"
#include "Renderer.h"

#include <QDebug>

SelectionImageActor::SelectionImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name)
{
//	addProp<SelectionImageProp>("ColorImageProp");
}

void SelectionImageActor::setImage(QSharedPointer<QImage> image)
{
	propByName<SelectionImageProp>("ColorImageProp")->setImage(image);
}

QSize SelectionImageActor::imageSize() const
{
	return propByName<SelectionImageProp>("ColorImageProp")->imageSize();
}