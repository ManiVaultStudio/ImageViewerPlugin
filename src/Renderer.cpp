#include "Renderer.h"
#include "ImageViewerWidget.h"

#include <QtMath>
#include <QMenu>
#include <QDebug>

#include "ColorImageActor.h"
#include "SelectionImageActor.h"
#include "SelectionPickerActor.h"

Renderer::Renderer(ImageViewerWidget* parentWidget) :
	hdps::Renderer(),
	_parentWidget(parentWidget),
	_actors(),
	_interactionMode(InteractionMode::Selection),
	_mouseEvents(),
	_pan(),
	_zoom(1.f),
	_zoomSensitivity(0.1f),
	_margin(25)
{
	addNamedColor("InterimSelectionOverlayColor", QColor(239, 130, 13, 50));
	addNamedColor("SelectionOutline", QColor(239, 130, 13, 255));

	createActors();
}

void Renderer::init()
{
	try
	{
		initializeActors();

		
	}
	catch (const std::exception& e)
	{
		qDebug() << "One or more  initialization failed:" << e.what();
	}
}

void Renderer::resize(QSize renderSize)
{
	qDebug() << "Renderer resize";

	zoomExtents();
}

void Renderer::render()
{
	renderActors();
}

void Renderer::destroy()
{
	destroyActors();
}

QVector<QSharedPointer<QMouseEvent>> Renderer::mouseEvents() const
{
	return _mouseEvents;
}

void Renderer::mousePressEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse press event";

	if (mouseEvent->buttons() & Qt::RightButton) {
		if (_interactionMode != InteractionMode::Navigation)
			setInteractionMode(InteractionMode::WindowLevel);
	}

	_mouseEvents.clear();
	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveMousePressEvents())
			actor->onMousePressEvent(mouseEvent);
	}
}

void Renderer::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse release event";

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveMouseReleaseEvents())
			actor->onMouseReleaseEvent(mouseEvent);
	}

	_mouseEvents.clear();
}

void Renderer::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	//qDebug() << "Mouse move event";

	/*
	if (mouseEvent->buttons() & Qt::RightButton) {
		if (_interactionMode != InteractionMode::Navigation && mouseEvent->pos() != _mouseEvents.first()->pos())
			setInteractionMode(InteractionMode::WindowLevel);
	}
	*/

	switch (mouseEvent->buttons())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					const auto noMouseEvents = _mouseEvents.size();

					if (noMouseEvents >= 2) {
						const auto pPrevious	= QVector2D(_mouseEvents[noMouseEvents - 2]->pos());
						const auto pCurrent		= QVector2D(_mouseEvents[noMouseEvents - 1]->pos());
						const auto vDelta		= (pCurrent - pPrevious) / _zoom;

						pan(vDelta);

						emit dirty();
					}
					
					break;
				}
			}
		}
	}

	_mouseEvents.push_back(QSharedPointer<QMouseEvent>::create(*mouseEvent));

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveMouseMoveEvents())
			actor->onMouseMoveEvent(mouseEvent);
	}
}

void Renderer::mouseWheelEvent(QWheelEvent* wheelEvent)
{
	//qDebug() << "Mouse wheel event";

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			const auto zoomCenter = QVector2D(wheelEvent->posF());

			if (wheelEvent->delta() < 0) {
				zoomAround(zoomCenter, 1.0f - _zoomSensitivity);
			}
			else {
				zoomAround(zoomCenter, 1.0f + _zoomSensitivity);
			}

			emit dirty();
			break;
		}

		default:
			break;
	}

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveMouseWheelEvents())
			actor->onMouseWheelEvent(wheelEvent);
	}
}

void Renderer::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent;

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		/*
		switch (keyEvent->key())
		{
			case Qt::Key::Key_R:
				_pixelSelection.setSelectionType(SelectionType::Rectangle);
				break;

			case Qt::Key::Key_B:
				_pixelSelection.setSelectionType(SelectionType::Brush);
				break;

			case Qt::Key::Key_L:
				_pixelSelection.setSelectionType(SelectionType::Lasso);
				break;

			case Qt::Key::Key_P:
				_pixelSelection.setSelectionType(SelectionType::Polygon);
				break;

			case Qt::Key::Key_Shift:
				_pixelSelection.setSelectionModifier(SelectionModifier::Add);
				break;

			case Qt::Key::Key_Control:
				_pixelSelection.setSelectionModifier(SelectionModifier::Remove);
				break;

			case Qt::Key::Key_Escape:
				_pixelSelection.abortSelection();
				break;

			case Qt::Key::Key_Space:
				setInteractionMode(InteractionMode::Navigation);
				break;

			default:
				break;
		}
		*/
	}

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveKeyPressEvents())
			actor->onKeyPressEvent(keyEvent);
	}
}

void Renderer::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent;

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		switch (keyEvent->key())
		{
			case Qt::Key::Key_Space:
				setInteractionMode(InteractionMode::Selection);
				break;

			default:
				break;
		}
	}

	for (auto key : _actors.keys()) {
		auto actor = _actors[key];

		if (actor->shouldReceiveKeyReleaseEvents())
			actor->onKeyReleaseEvent(keyEvent);
	}
}

QVector3D Renderer::screenPointToWorldPosition(const QMatrix4x4& modelViewMatrix, const QVector2D& screenPoint) const
{
	return QVector3D(screenPoint.x(), _parentWidget->height()- screenPoint.y(), 0).unproject(modelViewMatrix, projectionMatrix(), QRect(0, 0, _parentWidget->width(), _parentWidget->height()));
}

QVector2D Renderer::worldPositionToNormalizedScreenPoint(const QVector3D& position) const
{
	const auto clipSpacePos = projectionMatrix() * (viewMatrix() * QVector4D(position, 1.0));
	return (clipSpacePos.toVector3D() / clipSpacePos.w()).toVector2D();
}

QVector2D Renderer::worldPositionToScreenPoint(const QVector3D& position) const
{
	const auto normalizedScreenPoint	= worldPositionToNormalizedScreenPoint(position);
	const auto viewSize					= QVector2D(_parentWidget->width(), _parentWidget->height());

	return viewSize * ((QVector2D(1.0f, 1.0f) + normalizedScreenPoint) / 2.0f);
}

QVector2D Renderer::screenPointToNormalizedScreenPoint(const QVector2D& screenPoint) const
{
	const auto viewSize = QVector2D(_parentWidget->width(), _parentWidget->height());
	return QVector2D(-1.f, -1.f) + 2.f * (QVector2D(screenPoint.x(), _parentWidget->height() - screenPoint.y()) / viewSize);
}

QMatrix4x4 Renderer::screenToNormalizedScreenMatrix() const
{
	QMatrix4x4 translate, scale;

	translate.translate(-1.0f, -1.0f, 0.0f);
	scale.scale(2.0f / static_cast<float>(_parentWidget->width()), 2.0f / static_cast<float>(_parentWidget->height()), 1.0f);
	
	return translate * scale;
}

QMatrix4x4 Renderer::normalizedScreenToScreenMatrix() const
{
	QMatrix4x4 translate, scale;

	const auto size		= QSizeF(_parentWidget->size());
	const auto halfSize = 0.5f * size;

	
	scale.scale(halfSize.width(), halfSize.height(), 1.0f);
	translate.translate(size.width(), 1, 0.0f);

	//return translate * screenToNormalizedScreenMatrix().inverted();
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
	const auto halfSize = _parentWidget->size() / 2;

	QMatrix4x4 matrix;

	matrix.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -1000.0f, +1000.0f);

	return matrix;
}

void Renderer::pan(const QVector2D& delta)
{
	//qDebug() << "Pan by" << delta;

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

void Renderer::zoomAround(const QVector2D& screenPoint, const float& factor)
{
	zoomBy(factor);

	qDebug() << "Zoom around" << screenPoint << "by" << factor;

	const auto pWorld			= screenPointToWorldPosition(viewMatrix(), screenPoint);
	const auto pAnchor			= pWorld.toVector2D();
	const auto pPanOld			= _pan;
	const auto vPanOld			= pPanOld - pAnchor;
	const auto vPanNew			= factor * vPanOld;
	const auto vPanDelta		= vPanNew - vPanOld;

	pan(-vPanDelta);
}

void Renderer::zoomExtents()
{
	qDebug() << "Zoom extents";

	zoomToRectangle(QRectF(QPointF(), actorByName<ColorImageActor>("ColorImageActor")->imageSize()));
}

void Renderer::zoomToRectangle(const QRectF& rectangle)
{
	if (!rectangle.isValid())
		return;

	qDebug() << "Zoom to rectangle" << rectangle;

	resetView();

	const auto center	= rectangle.center();
	const auto factorX	= (_parentWidget->width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (_parentWidget->height() - 2 * _margin) / static_cast<float>(rectangle.height());
	
	zoomBy(factorX < factorY ? factorX : factorY);
	
	emit dirty();
}

void Renderer::zoomToSelection()
{
	/*
	auto* currentImageDataSet = _parentWidget->imageViewerPlugin()->currentImages();

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

void Renderer::setColorImage(std::shared_ptr<QImage> colorImage)
{
	bindOpenGLContext();

	colorImageActor()->setImage(colorImage);
	selectionPickerActor()->setImageSize(colorImage->size());
}

void Renderer::setSelectionImage(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds)
{
	auto worldSelectionBounds = QRect(selectionBounds.left(), selectionImage->height() - selectionBounds.bottom() - 1, selectionBounds.width() + 1, selectionBounds.height() + 1);

	worldSelectionBounds.translate(QPoint(-0.5f * static_cast<float>(selectionImage->width()), -0.5f * static_cast<float>(selectionImage->height())));

	actorByName<SelectionImageActor>("SelectionImageActor")->setImage(selectionImage);
}

float Renderer::selectionOpacity()
{
	return actorByName<SelectionImageActor>("SelectionImageActor")->opacity();
}

void Renderer::setSelectionOpacity(const float& selectionOpacity)
{
	actorByName<SelectionImageActor>("SelectionImageActor")->setOpacity(selectionOpacity);
}

InteractionMode Renderer::interactionMode() const
{
	return _interactionMode;
}

void Renderer::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(interactionMode);

	switch (interactionMode)
	{
		case InteractionMode::Navigation:
			_parentWidget->setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
		case InteractionMode::None:
		{
			_parentWidget->setCursor(Qt::ArrowCursor);
			break;
		}

		default:
			break;
	}
	
	_interactionMode = interactionMode;

	actorByName<ColorImageActor>("ColorImageActor")->setEnabled(_interactionMode == InteractionMode::WindowLevel);
	actorByName<SelectionPickerActor>("SelectionPickerActor")->setEnabled(_interactionMode == InteractionMode::Selection);
	//actorByName<SelectionPickerActor>("SelectionPickerActor")->setVisible(_interactionMode == InteractionMode::Selection);
}

void Renderer::bindOpenGLContext()
{
	_parentWidget->makeCurrent();
}

void Renderer::releaseOpenGLContext()
{
	_parentWidget->doneCurrent();
}

void Renderer::onActorChanged(Actor* actor)
{
	//qDebug() << "Actor" << actor->name() << "changed";

	emit dirty();
}

void Renderer::addActor(const QString& name, QSharedPointer<Actor> actor)
{
	_actors.insert(name, actor);

	connect(actor.get(), &Actor::changed, this, &Renderer::onActorChanged);
}

void Renderer::createActors()
{
	//qDebug() << "Creating actors";
	
	addActor("ColorImageActor", QSharedPointer<ColorImageActor>::create(this, "ColorImageActor"));
	addActor("SelectionImageActor", QSharedPointer<SelectionImageActor>::create(this, "SelectionImageActor"));
	addActor("SelectionPickerActor", QSharedPointer<SelectionPickerActor>::create(this, "SelectionPickerActor"));

	colorImageActor()->show();
	selectionImageActor()->show();
	selectionPickerActor()->hide();

	QMatrix4x4 modelMatrix;

	colorImageActor()->setModelMatrix(modelMatrix);

	modelMatrix.translate(0.0f, 0.0f, -1.0f);

	selectionImageActor()->setModelMatrix(modelMatrix);

	modelMatrix.translate(0.0f, 0.0f, -1.0f);

	selectionPickerActor()->setModelMatrix(modelMatrix);

	connect(colorImageActor(), &ColorImageActor::endWindowLevel, [&]() {
		setInteractionMode(InteractionMode::None);
	});
}

void Renderer::initializeActors()
{
	//qDebug() << "Initializing" << _actors.size() << "actor(s)";

	bindOpenGLContext();

	for (auto name : _actors.keys()) {
		_actors[name]->initialize();
	}

	connect(actorByName<ColorImageActor>("ColorImageActor"), &ColorImageActor::imageSizeChanged, this, [&]() { zoomExtents(); });
}

void Renderer::renderActors()
{
	//qDebug() << "Render" << _actors.size() << "actor(s)";

	bindOpenGLContext();

	for (auto name : _actors.keys()) {
		_actors[name]->render();
	}
}

void Renderer::destroyActors()
{
	//qDebug() << "Destroying" << _actors.size() << "actor(s)";

	bindOpenGLContext();

	for (auto name : _actors.keys()) {
		_actors[name]->destroy();
	}
}

ColorImageActor* Renderer::colorImageActor()
{
	return actorByName<ColorImageActor>("ColorImageActor");
}

SelectionPickerActor* Renderer::selectionPickerActor()
{
	return actorByName<SelectionPickerActor>("SelectionPickerActor");
}

SelectionImageActor* Renderer::selectionImageActor()
{
	return actorByName<SelectionImageActor>("SelectionImageActor");
}

QMenu* Renderer::viewMenu()
{
	auto* menu = new QMenu("View");

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

	return menu;
}

QMenu* Renderer::contextMenu()
{
	auto* menu = new QMenu("Context");

	menu->addMenu(viewMenu());
	menu->addSeparator();
	menu->addMenu(selectionPickerActor()->contextMenu());

	return menu;
}

QSize Renderer::viewSize() const
{
	return _parentWidget->size();
}

bool Renderer::allowsContextMenu()
{
	auto showContextMenu = false;

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
			return !selectionPickerActor()->isSelecting();
			break;

		default:
			break;
	}

	return false;
}

void Renderer::addNamedColor(const QString& name, const QColor& color)
{
	_colorMap.insert(name, color);
}

QColor Renderer::colorByName(const QString& name, const std::int32_t& alpha /*= -1*/) const
{
	if (!_colorMap.contains(name)) {
		return QColor(255, 255, 255, alpha);
	}

	auto color = _colorMap.value(name);

	if (alpha >= 0)
		color.setAlpha(alpha);

	return color;
}