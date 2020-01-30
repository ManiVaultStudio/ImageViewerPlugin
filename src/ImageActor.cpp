#include "ImageActor.h"

#include <QMouseEvent>
#include <QDebug>

#include "Renderer.h"

#include "ImageQuad.h"

ImageActor::ImageActor(Renderer* renderer, const QString& name) :
	Actor(renderer, name),
	_mousePositions()
{
	_receiveMouseEvents = static_cast<int>(MouseEvent::Press) | static_cast<int>(MouseEvent::Release) | static_cast<int>(MouseEvent::Move);

	addShape<ImageQuad>("ImageQuad");
}

bool ImageActor::isInitialized() const
{
	return true;
}

void ImageActor::setImage(std::shared_ptr<QImage> image)
{
	shape<ImageQuad>("ImageQuad")->setImage(image);
}

QSize ImageActor::imageSize()
{
	const auto size = shape<ImageQuad>("ImageQuad")->size();
	return QSize(size.width(), size.height());
}

float ImageActor::windowNormalized()
{
	return shape<ImageQuad>("ImageQuad")->windowNormalized();
}

float ImageActor::levelNormalized()
{
	return shape<ImageQuad>("ImageQuad")->levelNormalized();
}

void ImageActor::onMousePressEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMousePressEvent())
		return;

	//qDebug() << "Mouse press event for" << _name;

	_mousePositions.clear();
	_mousePositions.push_back(mouseEvent->pos());
}

void ImageActor::onMouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseReleaseEvent())
		return;

	//qDebug() << "Mouse release event for" << _name;
}

void ImageActor::onMouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!mayProcessMouseMoveEvent())
		return;

	//qDebug() << "Mouse move event for" << _name;

	_mousePositions.push_back(mouseEvent->pos());

	if (_mousePositions.size() > 1 && mouseEvent->buttons() & Qt::RightButton) {
		auto* imageQuad = shape<ImageQuad>("ImageQuad");

		const auto mousePosition0	= _mousePositions[_mousePositions.size() - 2];
		const auto mousePosition1	= _mousePositions.back();
		const auto deltaWindow		= (mousePosition1.x() - mousePosition0.x()) / 150.f;
		const auto deltaLevel		= -(mousePosition1.y() - mousePosition0.y()) / 150.f;
		const auto window			= std::clamp(imageQuad->windowNormalized() + deltaWindow, 0.0f, 1.0f);
		const auto level			= std::clamp(imageQuad->levelNormalized() + deltaLevel, 0.0f, 1.0f);

		shape<ImageQuad>("ImageQuad")->setWindowLevel(window, level);
	}
}