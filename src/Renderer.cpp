#include "Renderer.h"
#include "ViewerWidget.h"

#include <QtMath>
#include <QMenu>
#include <QDebug>
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>

Renderer::Renderer(QOpenGLWidget* parent) :
	QObject(parent),
	hdps::Renderer(),
	_mousePositions(),
	_mouseButtons(),
	_keys(),
	_pan(),
	_zoom(1.f),
	_zoomSensitivity(0.1f),
	_margin(25)
{
}

void Renderer::init()
{
}

void Renderer::render()
{
	static_cast<QOpenGLWidget*>(parent())->update();
}

bool Renderer::interacting() const
{
	return _keys & Qt::Key_Space;
}

void Renderer::handleEvent(QEvent* event)
{
	/*
	_mouseButtons = mouseEvent->buttons();

	_mousePositions << mouseEvent->pos();

	//qDebug() << "Mouse press event";

	if (mouseEvent->buttons() & Qt::RightButton) {
		if (_interactionMode != InteractionMode::Navigation)
			setInteractionMode(InteractionMode::WindowLevel);
	}

	_mouseEvents.clear();
	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));
	*/

	/*
	_mousePositions.clear();
	*/

	/*
	if (mouseEvent->buttons() & Qt::RightButton) {
		if (_interactionMode != InteractionMode::Navigation && mouseEvent->pos() != _mouseEvents.first()->pos())
			setInteractionMode(InteractionMode::WindowLevel);
	}
	*/

	/*
	switch (mouseEvent->buttons())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					const auto noMouseEvents = _mousePositions.size();

					if (noMouseEvents >= 2) {
						const auto pPrevious	= QVector2D(_mousePositions[noMouseEvents - 2]);
						const auto pCurrent		= QVector2D(_mousePositions[noMouseEvents - 1]);
						const auto vDelta		= (pCurrent - pPrevious) / _zoom;

						pan(vDelta);

						static_cast<QOpenGLWidget*>(parent())->update();
					}

					break;
				}
			}
		}
	}
	*/

	/*
	//qDebug() << "Mouse wheel event";

	/*
	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			const auto zoomCenter = wheelEvent->pos();

			if (wheelEvent->delta() < 0) {
				zoomAround(zoomCenter, 1.0f - _zoomSensitivity);
			}
			else {
				zoomAround(zoomCenter, 1.0f + _zoomSensitivity);
			}

			emit becameDirty();
			static_cast<QOpenGLWidget*>(parent())->update();
			break;
		}

		default:
			break;
	}
	if (mouseEvent->buttons() & Qt::LeftButton && _keys & Qt::Key_Space && !_mousePositions.isEmpty()) {
		qDebug() << "mouseMoveEvent";
		_mousePositions << mouseEvent->pos();

		renderer->pan(QVector2D(mouseEvent->pos() - _mousePositions.last()));
	}
	*/
}

QVector3D Renderer::screenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QPoint& screenPoint) const
{
	return QVector3D(screenPoint.x(), parentWidgetSize().height()- screenPoint.y(), 0).unproject(modelViewMatrix, projectionMatrix(), QRect(0, 0, parentWidgetSize().width(), parentWidgetSize().height()));
}

QVector2D Renderer::worldPositionToNormalizedScreenPoint(const QVector3D& position) const
{
	const auto clipSpacePos = projectionMatrix() * (viewMatrix() * QVector4D(position, 1.0));
	return (clipSpacePos.toVector3D() / clipSpacePos.w()).toVector2D();
}

QPoint Renderer::worldPositionToScreenPoint(const QVector3D& position) const
{
	const auto normalizedScreenPoint	= worldPositionToNormalizedScreenPoint(position);
	const auto viewSize					= QVector2D(parentWidgetSize().width(), parentWidgetSize().height());

	return (viewSize * ((QVector2D(1.0f, 1.0f) + normalizedScreenPoint) / 2.0f)).toPoint();
}

QVector2D Renderer::screenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const
{
	const auto viewSize = QVector2D(parentWidgetSize().width(), parentWidgetSize().height());
	return QVector2D(-1.f, -1.f) + 2.f * (QVector2D(screenPoint.x(), parentWidgetSize().height() - screenPoint.y()) / viewSize);
}

QMatrix4x4 Renderer::screenToNormalizedScreenMatrix() const
{
	QMatrix4x4 translate, scale;

	translate.translate(-1.0f, -1.0f, 0.0f);
	scale.scale(2.0f / static_cast<float>(parentWidgetSize().width()), 2.0f / static_cast<float>(parentWidgetSize().height()), 1.0f);
	
	return translate * scale;
}

QMatrix4x4 Renderer::normalizedScreenToScreenMatrix() const
{
	QMatrix4x4 translate, scale;

	const auto size		= QSizeF(parentWidgetSize());
	const auto halfSize = 0.5f * size;

	
	scale.scale(halfSize.width(), halfSize.height(), 1.0f);
	translate.translate(size.width(), 1, 0.0f);

	return translate * scale;// *;
}

QMatrix4x4 Renderer::viewMatrix() const
{
	QMatrix4x4 lookAt, scale;

	lookAt.lookAt(QVector3D(_pan.x(), _pan.y(), -1), QVector3D(_pan.x(), _pan.y(), 0), QVector3D(0, 1, 0));
	scale.scale(_zoom);

	return scale * lookAt;
}

QMatrix4x4 Renderer::projectionMatrix() const
{
	const auto halfSize = parentWidgetSize() / 2;

	QMatrix4x4 matrix;

	matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

	return matrix;
}

void Renderer::pan(const QVector2D& delta)
{
	qDebug() << "Pan by" << delta;

	_pan.setX(_pan.x() + delta.x());
	_pan.setY(_pan.y() + delta.y());
}

float Renderer::zoom() const
{
	return _zoom;
}

void Renderer::zoomBy(const float& factor)
{
	if (factor == 0.f)
		return;

	qDebug() << "Zoom by" << factor << "to" << _zoom;

	_zoom *= factor;
}

void Renderer::zoomAround(const QPoint& screenPoint, const float& factor)
{
	zoomBy(factor);

	qDebug() << "Zoom around" << screenPoint << "by" << factor;

	const auto pWorld		= screenPointToWorldPosition(viewMatrix(), screenPoint);
	const auto pAnchor		= pWorld.toVector2D();
	const auto pPanOld		= _pan;
	const auto vPanOld		= pPanOld - pAnchor;
	const auto vPanNew		= factor * vPanOld;
	const auto vPanDelta	= vPanNew - vPanOld;

	pan(-vPanDelta);
}

void Renderer::zoomToRectangle(const QRectF& rectangle)
{
	if (!rectangle.isValid())
		return;

	qDebug() << "Zoom to rectangle" << rectangle;

	resetView();

	const auto center	= rectangle.center();
	const auto factorX	= (parentWidgetSize().width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (parentWidgetSize().height() - 2 * _margin) / static_cast<float>(rectangle.height());
	
	zoomBy(factorX < factorY ? factorX : factorY);
}

void Renderer::zoomToSelection()
{
	/*
	auto* currentImageDataSet = parentSize().imageViewerPlugin()->currentImages();

	if (currentImageDataSet == nullptr)
		return;

	qDebug() << "Zoom to selection";

	zoomToRectangle(QRectF(currentImageDataSet->selectionBounds(true)));
	*/
}

void Renderer::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);

	_zoom = 1.f;
}

void Renderer::bindOpenGLContext()
{
	parentWidget()->makeCurrent();
}

void Renderer::releaseOpenGLContext()
{
	parentWidget()->doneCurrent();
}

QMenu* Renderer::viewMenu()
{
	auto* menu = new QMenu("View");

	/*
	auto* zoomToExtentsAction = new QAction("Zoom extents");
	//auto* zoomToSelectionAction = new QAction("Zoom to selection");
	auto* resetWindowLevelAction = new QAction("Reset window/level");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");
	//zoomToSelectionAction->setToolTip("Zoom to selection boundaries");
	resetWindowLevelAction->setToolTip("Reset window/level to default values");

	resetWindowLevelAction->setEnabled(colorImageActor()->windowNormalized() < 1.f && colorImageActor()->levelNormalized() != 0.5f);

	connect(zoomToExtentsAction, &QAction::triggered, this, &Renderer::zoomExtents);
	//connect(zoomToSelectionAction, &QAction::triggered, this, &Renderer::zoomToSelection);
	connect(resetWindowLevelAction, &QAction::triggered, [&]() {
		this->actorByName<ColorImageActor>("ColorImageActor")->resetWindowLevel();
	});

	menu->addAction(zoomToExtentsAction);
	//menu->addAction(zoomToSelectionAction);
	menu->addSeparator();
	menu->addAction(resetWindowLevelAction);
	*/

	return menu;
}

QMenu* Renderer::contextMenu()
{
	auto* menu = new QMenu("Context");

	menu->addMenu(viewMenu());
	menu->addSeparator();
	//menu->addMenu(selectionPickerActor()->contextMenu());

	return menu;
}

bool Renderer::allowsContextMenu()
{
	auto showContextMenu = false;

	/*
	switch (interactionMode())
	{
		case InteractionMode::Navigation:
			break;

		case InteractionMode::WindowLevel:
			return colorImageActor()->mouseEvents().size() == 1;
			break;

		case InteractionMode::None:
			return true;

		case InteractionMode::Selection:
			//return !selectionPickerActor()->isSelecting();
			break;

		default:
			break;
	}
	*/

	return false;
}

QOpenGLWidget* Renderer::parentWidget() const
{
	return dynamic_cast<QOpenGLWidget*>(parent());
}

QOpenGLContext* Renderer::openGLContext() const
{
	return parentWidget()->context();
}

QSize Renderer::parentWidgetSize() const
{
	return parentWidget()->size();
}