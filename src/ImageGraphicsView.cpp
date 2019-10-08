#include "ImageGraphicsView.h"
#include "ImageWidget.h"
#include "ImageViewerPlugin.h"

#include <QGraphicsProxyWidget>
#include <QMenu>
#include <QScrollBar>

ImageGraphicsView::ImageGraphicsView(ImageViewerPlugin* imageViewerPlugin, QWidget *parent /*= 0*/) :
	QGraphicsView(parent),
	_imageViewerPlugin(imageViewerPlugin),
	_graphicsScene(nullptr),
	_interactionMode(InteractionMode::Selection),
	_interactionStartMousePosition(),
	_lastMousePosition(),
	_panning(false),
	_scale(1),
	_zoomSensitivity(0.1)
{
	//setDragMode(QGraphicsView::ScrollHandDrag);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	
	_graphicsScene = new QGraphicsScene(this);

	_graphicsScene->setBackgroundBrush(Qt::darkGray);

	const auto size = 10000;

	_graphicsScene->setSceneRect(-size / 2, -size / 2, size, size);

	_imageWidget = new ImageWidget();

	auto* graphicsProxyWidget = new QGraphicsProxyWidget();
	graphicsProxyWidget->setWidget(_imageWidget);

	_graphicsScene->addItem(graphicsProxyWidget);

	setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	setScene(_graphicsScene);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageGraphicsView::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageGraphicsView::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageGraphicsView::onSelectionImageChanged);
	connect(_imageWidget, &ImageWidget::rendered, this, &ImageGraphicsView::onImageWidgetRendered);
}

InteractionMode ImageGraphicsView::interactionMode() const
{
	return _interactionMode;
}

void ImageGraphicsView::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	_interactionMode = interactionMode;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(_interactionMode);

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
			viewport()->setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
			viewport()->setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}
}

void ImageGraphicsView::zoomToExtents()
{
	fitInView(_imageWidget->rect(), Qt::KeepAspectRatio);
}

QMenu* ImageGraphicsView::windowLevelMenu()
{
	auto* windowLevelMenu = new QMenu("Window/level");

	auto* resetAction = new QAction("Reset");

	resetAction->setToolTip("Reset window and level");

	connect(resetAction, &QAction::triggered, _imageWidget, &ImageWidget::resetWindowLevel);

	windowLevelMenu->addAction(resetAction);

	return windowLevelMenu;
}

QMenu* ImageGraphicsView::viewMenu()
{
	auto* viewMenu = new QMenu("View");

	auto* zoomToExtentsAction = new QAction("Zoom extents");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageGraphicsView::zoomToExtents);

	viewMenu->addAction(zoomToExtentsAction);

	return viewMenu;
}

QMenu* ImageGraphicsView::selectionMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction = new QAction("Rectangle");
	auto* brushSelectionAction = new QAction("Brush");
	auto* freehandSelectionAction = new QAction("Freehand", this);
	auto* clearSelectionAction = new QAction("Clear");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { _imageWidget->setSelectionType(SelectionType::Rectangle);  });
	connect(brushSelectionAction, &QAction::triggered, [this]() { _imageWidget->setSelectionType(SelectionType::Brush);  });
	connect(freehandSelectionAction, &QAction::triggered, [this]() { _imageWidget->setSelectionType(SelectionType::Freehand);  });
	//connect(clearSelectionAction, &QAction::triggered, [this]() { myGLWidget->clearSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_imageWidget->selectionType() == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_imageWidget->selectionType() == SelectionType::Brush);

	freehandSelectionAction->setEnabled(false);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(clearSelectionAction);

	return selectionMenu;
}

void ImageGraphicsView::contextMenuEvent(QContextMenuEvent* contextMenuEvent)
{
	auto* contextMenu = new QMenu();

	//if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
	contextMenu->addMenu(windowLevelMenu());
	contextMenu->addSeparator();
	contextMenu->addMenu(viewMenu());
	contextMenu->addSeparator();
	contextMenu->addMenu(selectionMenu());
	
	contextMenu->exec(mapToGlobal(contextMenuEvent->pos()));
}

void ImageGraphicsView::onCurrentDatasetChanged(const QString& currentDataset)
{
//	_imageWidget->setSelectionImage(std::vector<std::uint8_t>({ 0 }), QSize(1, 1));
}

void ImageGraphicsView::onDisplayImageChanged(std::vector<std::uint16_t>& displayImage, const QSize& imageSize, const double& imageMin, const double& imageMax)
{
	qDebug() << "Display image changed";

	auto reset = false;

	if (imageSize != _imageWidget->size()) {
		reset = true;
	}

	_imageWidget->setDisplayImage(displayImage, imageSize, imageMin, imageMax);

	if (reset) {
		zoomToExtents();
		_imageWidget->resetWindowLevel();
	}
}

void ImageGraphicsView::onSelectionImageChanged(std::vector<std::uint8_t>& selectionImage, const QSize& imageSize)
{
	_imageWidget->setSelectionImage(selectionImage, imageSize);
}

void ImageGraphicsView::onImageWidgetRendered()
{
	
}

void ImageGraphicsView::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		if (_interactionMode == InteractionMode::Selection) {
			switch (keyEvent->key())
			{
			case Qt::Key::Key_R:
			{
				_imageWidget->setSelectionType(SelectionType::Rectangle);
				break;
			}

			case Qt::Key::Key_B:
			{
				_imageWidget->setSelectionType(SelectionType::Brush);
				break;
			}

			case Qt::Key::Key_F:
			{
				_imageWidget->setSelectionType(SelectionType::Freehand);
				break;
			}

			case Qt::Key::Key_Shift:
			{
				//if (myGLWidget->selectionModifier() != SelectionModifier::Remove)
				_imageWidget->setSelectionModifier(SelectionModifier::Add);
				break;
			}

			case Qt::Key::Key_Control:
			{
				//if (myGLWidget->selectionModifier() != SelectionModifier::Add)
				_imageWidget->setSelectionModifier(SelectionModifier::Remove);
				break;
			}

			case Qt::Key::Key_Space:
			{
				setInteractionMode(InteractionMode::Navigation);
				break;
			}

			default:
				break;
			}
		}
	}

	QWidget::keyPressEvent(keyEvent);
}

void ImageGraphicsView::keyReleaseEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key release event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
		switch (keyEvent->key())
		{
			case Qt::Key::Key_Shift:
			case Qt::Key::Key_Control:
			{
				if (_imageWidget->selectionType() != SelectionType::Brush) {
					_imageWidget->setSelectionModifier(SelectionModifier::Replace);
				}

				break;
			}

			case Qt::Key::Key_Space:
			{
				setInteractionMode(InteractionMode::Selection);
				break;

			}
			default:
				break;
		}
	}

	QWidget::keyReleaseEvent(keyEvent);
}

void ImageGraphicsView::mousePressEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					_panning = true;

					viewport()->setCursor(Qt::ClosedHandCursor);
					
					mouseEvent->accept();
					break;
				}

				case InteractionMode::Selection:
				{
					break;
				}

				case InteractionMode::WindowLevel:
					break;
				default:
					break;
			}
			break;
		}

		case Qt::RightButton:
		{
			setInteractionMode(InteractionMode::WindowLevel);
			break;
		}

		default:
			break;
	}

	_interactionStartMousePosition = mouseEvent->pos();

	QGraphicsView::mousePressEvent(mouseEvent);
}

void ImageGraphicsView::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			if (mouseEvent->button() == Qt::LeftButton) {
				//QPointF mouseDelta = mapToScene(mouseEvent->pos()) - mapToScene(_lastMousePosition);
				//pan(mouseDelta);

				//horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (mouseEvent->x() - _lastMousePosition.x()));
				//verticalScrollBar()->setValue(verticalScrollBar()->value() - (mouseEvent->y() - _lastMousePosition.y()));
				
				mouseEvent->accept();
			}

			break;
		}
		
		case InteractionMode::Selection:
			break;

		case InteractionMode::WindowLevel:
		{
			const auto deltaWindow	= (mouseEvent->pos().x() - _lastMousePosition.x()) / 200.0;
			const auto deltaLevel	= -(mouseEvent->pos().y() - _lastMousePosition.y()) / 200.0;
			const auto window		= std::max<double>(0, std::min<double>(_imageWidget->window() + deltaWindow, 1.0f));
			const auto level		= std::max<double>(0, std::min<double>(_imageWidget->level() + deltaLevel, 1.0f));

			_imageWidget->setWindowLevel(window, level);

			break;
		}

		default:
			break;
	}

	_lastMousePosition = mouseEvent->pos();

	QGraphicsView::mouseMoveEvent(mouseEvent);
}

void ImageGraphicsView::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					_panning = false;

					viewport()->setCursor(Qt::OpenHandCursor);

					mouseEvent->accept();
					break;
				}

				case InteractionMode::Selection:
				{
					break;
				}

				case InteractionMode::WindowLevel:
					break;
				default:
					break;
			}
			break;
		}

		case Qt::RightButton:
		{
			setInteractionMode(InteractionMode::WindowLevel);
			break;
		}

		default:
			break;
	}

	QGraphicsView::mouseReleaseEvent(mouseEvent);
}

void ImageGraphicsView::wheelEvent(QWheelEvent* wheelEvent)
{
	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			wheelEvent->angleDelta().y() > 0 ? zoomIn() : zoomOut();
			break;
		}

		case InteractionMode::Selection:
			break;

		case InteractionMode::WindowLevel:
			break;

		default:
			break;
	}

	QGraphicsView::wheelEvent(wheelEvent);
}

void ImageGraphicsView::resizeEvent(QResizeEvent* resizeEvent)
{
	zoomToExtents();

	QGraphicsView::resizeEvent(resizeEvent);
}

void ImageGraphicsView::pan(QPointF delta)
{
	// Scale the pan amount by the current zoom.
	delta *= _scale;
	delta *= 4;

	// Have panning be anchored from the mouse.
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	QPoint newCenter(viewport()->rect().width() / 2 - delta.x(), viewport()->rect().height() / 2 - delta.y());
	centerOn(mapToScene(newCenter));

	// For zooming to anchor from the view center.
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void ImageGraphicsView::zoom(const float& scaleFactor)
{
	scale(scaleFactor, scaleFactor);
	_scale *= scaleFactor;
}

void ImageGraphicsView::zoomIn()
{
	zoom(1 + _zoomSensitivity);
}

void ImageGraphicsView::zoomOut()
{
	zoom(1 - _zoomSensitivity);
}


//setInteractionMode(InteractionMode::Selection);

	/*
	//if (!imageInitialized())
	return;

	qDebug() << "Mouse release event";

	//if (_interactionMode != InteractionMode::WindowLevel) {
	if (_imageViewerPlugin->selectable()) {
		if (mouseEvent->button() == Qt::RightButton)
		{

		}
	}
	//}
	/*
	switch (_interactionMode)
	{
	case InteractionMode::Navigation:
	{
		QWidget::setCursor(Qt::OpenHandCursor);
		break;
	}

	case InteractionMode::Selection:
	{
		if (_imageViewerPlugin->selectable()) {
			if (_selecting) {
				if (_imageViewerPlugin->selectable()) {
					enableSelection(false);
					updateSelection();
				}

				commitSelection();
			}
		}
		break;
	}

	case InteractionMode::WindowLevel:
	{
		setInteractionMode(InteractionMode::Selection);
		break;
	}

	default:
		break;
	}

	update();
	*/