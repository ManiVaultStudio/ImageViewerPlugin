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

	addShape<ImageQuad>("Quad");
}

bool ImageActor::isInitialized() const
{
	return true;
}

void ImageActor::setImage(std::shared_ptr<QImage> image)
{
	shape<ImageQuad>("Quad")->setImage(image);
}

QSize ImageActor::imageSize()
{
	return dynamic_cast<ImageQuad*>(_shapes["Quad"].get())->size();
}

float ImageActor::windowNormalized()
{
	return shape<ImageQuad>("Quad")->windowNormalized();
}

float ImageActor::levelNormalized()
{
	return shape<ImageQuad>("Quad")->levelNormalized();
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
		auto* imageQuad = shape<ImageQuad>("Quad");

		const auto mousePosition0	= _mousePositions[_mousePositions.size() - 2];
		const auto mousePosition1	= _mousePositions.back();
		const auto deltaWindow		= (mousePosition1.x() - mousePosition0.x()) / 150.f;
		const auto deltaLevel		= -(mousePosition1.y() - mousePosition0.y()) / 150.f;
		const auto window			= std::clamp(imageQuad->windowNormalized() + deltaWindow, 0.0f, 1.0f);
		const auto level			= std::clamp(imageQuad->levelNormalized() + deltaLevel, 0.0f, 1.0f);

		shape<ImageQuad>("Quad")->setWindowLevel(window, level);
	}
}