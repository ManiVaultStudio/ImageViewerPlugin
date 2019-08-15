#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include <vector>

#include <QSize>
#include <QDebug>

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin),
	_texture(QImage("C:\\Users\\tkroes\\Desktop\\harakka_timo-activation_model.jpg"))
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &ImageViewerWidget::onSelectedPointsChanged);
}

void ImageViewerWidget::onCurrentImageChanged(const QString & dataSetName, const int & imageIndex)
{
	if (imageIndex < 0)
		return;

	const auto imageCollectionType = _imageViewerPlugin->imageCollectionType();

	qDebug() << QString("Loading image %1 from %2 (%3)").arg(QString::number(imageIndex), dataSetName, imageCollectionType);
}

void ImageViewerWidget::onSelectedPointsChanged()
{
}

void ImageViewerWidget::loadImage(const QString& dataSetName, const int& imageIndex)
{
	
}

void ImageViewerWidget::computeSequenceAverageImage()
{
}

void ImageViewerWidget::initializeGL()
{
	initializeOpenGLFunctions();

	/*
	connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &ImageViewerWidget::cleanup);

	_colormapWidget.move(width() - 71, 10);
	_colormapWidget.show();

	QObject::connect(&_colormapWidget, &ColormapWidget::colormapSelected, this, &ImageViewerWidget::colormapChanged);
	QObject::connect(&_colormapWidget, &ColormapWidget::discreteSelected, this, &ImageViewerWidget::colormapdiscreteChanged);

	_pointRenderer.init();
	_densityRenderer.init();
	_selectionRenderer.init();

	// Set a default color map for both renderers
	_pointRenderer.setScalarEffect(PointEffect::Color);
	_pointRenderer.setColormap(_colormapWidget.getActiveColormap());
	_densityRenderer.setColormap(_colormapWidget.getActiveColormap());

	_isInitialized = true;
	emit initialized();
	*/
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";
	
	/*
	_windowSize.setWidth(w);
	_windowSize.setHeight(h);

	_pointRenderer.resize(QSize(w, h));
	_densityRenderer.resize(QSize(w, h));
	_selectionRenderer.resize(QSize(w, h));

	toNormalisedCoordinates = Matrix3f(1.0f / w, 0, 0, 1.0f / h, 0, 0);

	int size = w < h ? w : h;

	float wAspect = (float)w / size;
	float hAspect = (float)h / size;
	float wDiff = ((wAspect - 1) / 2.0);
	float hDiff = ((hAspect - 1) / 2.0);

	toIsotropicCoordinates = Matrix3f(wAspect, 0, 0, hAspect, -wDiff, -hDiff);
	qDebug() << "Done resizing scatterplot";

	if (_colormapWidget._isOpen)
	{
		_colormapWidget.move(width() - (64 + 14 + 15 * 36 + 15), 10);
	}
	else {
		_colormapWidget.move(width() - 71, 10);
	}
	_colormapWidget.setColormap(0, true);
	*/
}

void ImageViewerWidget::paintGL()
{
	/*
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, filename);

	glLoadIdentity();
	glTranslatef(xx, yy, 0.0);
	glRotatef(angle, 0.0, 0.0, 1.0);
	glTranslatef(-xx, -yy, 0.0);

	// Draw a textured quad
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(xx, yy);
	glTexCoord2f(0, 1); glVertex2f(xx, yy + hh);
	glTexCoord2f(1, 1); glVertex2f(xx + ww, yy + hh);
	glTexCoord2f(1, 0); glVertex2f(xx + ww, yy);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glEnd();
	*/

	/*
	// Bind the framebuffer belonging to the widget
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

	// Clear the widget to the background color
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset the blending function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch (_renderMode)
	{
	case SCATTERPLOT: _pointRenderer.render(); break;
	case DENSITY:
	{
		_densityRenderer.setRenderMode(DensityRenderer::DENSITY);
		_densityRenderer.render();
		break;
	}
	case LANDSCAPE:
	{
		_densityRenderer.setRenderMode(DensityRenderer::LANDSCAPE);
		_densityRenderer.render();
		break;
	}
	}
	_selectionRenderer.render();
	*/
}

void ImageViewerWidget::mousePressEvent(QMouseEvent *event)
{
	/*
	_selecting = true;

	Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
	_selection.setStart(point);
	*/
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
	/*
	if (!_selecting) return;

	Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
	_selection.setEnd(point);

	onSelecting(_selection);

	update();
	*/
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{
	/*
	_selecting = false;

	Vector2f point = toNormalisedCoordinates * Vector2f(event->x(), _windowSize.height() - event->y());
	_selection.setEnd(point);

	onSelection(_selection);

	update();
	*/
}