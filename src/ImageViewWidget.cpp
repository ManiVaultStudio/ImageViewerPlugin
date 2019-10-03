#include <QtWidgets>
#include <QGraphicsProxyWidget>

#include "ImageViewerPlugin.h"
#include "ImageViewWidget.h"
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

	myGLWidget = new ImageCanvasWidget();

	auto* proxy = new QGraphicsProxyWidget();
	proxy->setWidget(myGLWidget);
	//add to scene
	_scene->addItem(proxy);

	QSurfaceFormat format;
	format.setSamples(4);
	myGLWidget->setFormat(format);

	//view->setViewport(myGLWidget);
	_imageView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	_imageView->setScene(_scene);

	mainLayout->addWidget(_imageView);
	setLayout(mainLayout);

	setWindowTitle(tr("GraphicsView + OpenGL Test Unit"));

	//view->scene()->installEventFilter(this);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, myGLWidget, &ImageCanvasWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, [=]() {
		const auto size = 10000;

	_scene->update(-size / 2, -size / 2, size, size);
	});
}



