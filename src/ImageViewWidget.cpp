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

	_imageView = new ImageView(this);
	_scene = new QGraphicsScene(_imageView);

	_imageView->setDragMode(QGraphicsView::ScrollHandDrag);

	_imageView->setRenderHints(QPainter::Antialiasing);
	_scene->setBackgroundBrush(Qt::blue);

	_scene->setSceneRect(-500, -500, 1000, 1000);

	myGLWidget = new GLWidget();

	auto* proxy = new QGraphicsProxyWidget();
	proxy->setWidget(myGLWidget);
	//add to scene
	_scene->addItem(proxy);
	QOpenGLWidget *oglw = qobject_cast<QOpenGLWidget *>(myGLWidget);

	QSurfaceFormat format;
	format.setSamples(4);
	oglw->setFormat(format);

	//view->setViewport(myGLWidget);
	//view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

	_imageView->setScene(_scene);

	mainLayout->addWidget(_imageView);
	setLayout(mainLayout);

	setWindowTitle(tr("GraphicsView + OpenGL Test Unit"));

	//view->scene()->installEventFilter(this);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewWidget::onDisplayImageChanged);
}

void ImageViewWidget::SetTestImage()
{
	QImage input_image(QString("C:\\Users\\tkroes\\Downloads\\1.jpg"));
	GLWidget *glw = qobject_cast<GLWidget *>(myGLWidget);
	glw->SetImage(input_image);
}

void ImageViewWidget::onDisplayImageChanged(const QSize& imageSize, TextureData& displayImage)
{
	/*
	qDebug() << "Display image changed";

	auto shouldZoomExtents = false;

	if (imageSize != _imageSize) {
		_imageSize = imageSize;

		setupTextures();

		shouldZoomExtents = true;
	}

	_textures["image"]->setData(QOpenGLTexture::PixelFormat::Red, QOpenGLTexture::PixelType::UInt16, static_cast<void*>(displayImage.data()));

	update();

	if (shouldZoomExtents)
		zoomExtents();
	*/
}