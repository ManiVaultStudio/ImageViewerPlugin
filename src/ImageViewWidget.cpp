#include <QtWidgets>
#include <QGraphicsProxyWidget>
#include <QMenu>

#include "ImageViewWidget.h"
#include "ImageViewerPlugin.h"
#include "ImageView.h"
#include "ImageCanvasWidget.h"

ImageViewWidget::ImageViewWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin)
{
	QVBoxLayout *mainLayout = new QVBoxLayout;

	mainLayout->setSpacing(0);
	mainLayout->setMargin(0);

	_imageView = new ImageView(this);
	_scene = new QGraphicsScene(_imageView);

	_imageView->setDragMode(QGraphicsView::ScrollHandDrag);

	_imageView->setRenderHints(QPainter::Antialiasing);
	_scene->setBackgroundBrush(Qt::darkGray);

	const auto size = 10000;

	_scene->setSceneRect(-size / 2, -size / 2, size, size);

	_imageWidget = new ImageCanvasWidget();

	auto* proxy = new QGraphicsProxyWidget();
	proxy->setWidget(_imageWidget);
	//add to scene
	_scene->addItem(proxy);

	QSurfaceFormat format;
	format.setSamples(4);
	_imageWidget->setFormat(format);

	//view->setViewport(myGLWidget);
	_imageView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	_imageView->setScene(_scene);

	mainLayout->addWidget(_imageView);
	setLayout(mainLayout);

	setWindowTitle(tr("GraphicsView + OpenGL Test Unit"));

	//view->scene()->installEventFilter(this);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, _imageWidget, &ImageCanvasWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, [=]() { _scene->update(_scene->sceneRect()); });
}

void ImageViewWidget::onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage, const double& imageMin, const double& imageMax)
{
	qDebug() << "Display image changed";

	auto shouldZoomExtents = false;

	/*
	if (imageSize != _imageSize) {
		_imageSize = imageSize;

		setupTextures();

		shouldZoomExtents = true;
	}

	_textures["image"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(displayImage.data()));
	*/

	_imageWidget->setImage(displayImage, imageSize, imageMin, imageMax);

	/*
	_imageView->update();
	const auto size = 10000;

	_scene->update(-size / 2, -size / 2, size, size);

	update();
	*/
	/*
	if (shouldZoomExtents)
		zoomExtents();
	*/

	_imageMin = imageMin;
	_imageMax = imageMax;
}

void ImageViewWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	//qDebug() << "Key press event" << keyEvent->key();

	if (keyEvent->isAutoRepeat())
	{
		keyEvent->ignore();
	}
	else
	{
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
			_imageWidget->setInteractionMode(InteractionMode::Navigation);
			break;
		}

		default:
			break;
		}
	}

	QWidget::keyPressEvent(keyEvent);
}

void ImageViewWidget::keyReleaseEvent(QKeyEvent* keyEvent)
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
			_imageWidget->setInteractionMode(InteractionMode::Selection);
			break;

		}
		default:
			break;
		}
	}

	QWidget::keyReleaseEvent(keyEvent);
}

QMenu* ImageViewWidget::contextMenu()
{
	auto* contextMenu = new QMenu();

	//if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
	contextMenu->addMenu(viewMenu());
	contextMenu->addSeparator();
	contextMenu->addMenu(selectionMenu());
	//}

	return contextMenu;
}

QMenu* ImageViewWidget::viewMenu()
{
	auto* viewMenu = new QMenu("View");

	auto* zoomToExtentsAction = new QAction("Zoom extents");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");

	//connect(zoomToExtentsAction, &QAction::triggered, this, &ImageCanvasWidget::zoomExtents);

	viewMenu->addAction(zoomToExtentsAction);

	return viewMenu;
}

QMenu* ImageViewWidget::selectionMenu()
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