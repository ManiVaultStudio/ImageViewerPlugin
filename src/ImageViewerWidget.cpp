#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"

#include <vector>

#include <QSize>
#include <QDebug>
#include <QMenu>

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin),
	_texture(QOpenGLTexture::Target2D),
	_selectionOverlayTexture(QOpenGLTexture::Target2D),
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(10)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageIdsChanged, this, &ImageViewerWidget::onDisplayImageIdsChanged);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDataSetNameChanged), this, &ImageViewerWidget::zoomExtents);
}

void ImageViewerWidget::onDisplayImageIdsChanged()
{
	const auto imageSize		= _imageViewerPlugin->imageSize();
	const auto noPixels			= imageSize.width() * imageSize.height();
	const auto type				= _imageViewerPlugin->imageCollectionType();
	const auto displayImageIds	= _imageViewerPlugin->displayImageIds();
	const auto noDisplayImages	= displayImageIds.size();
	const auto noImages			= _imageViewerPlugin->noImages();
	const auto width			= imageSize.width();
	const auto height			= imageSize.height();
	
	if (QSize(_texture.width(), _texture.height()) != imageSize) {
		setupTextures(imageSize);
	}

	PointsPlugin& pointsData = _imageViewerPlugin->pointsData();
	
	std::vector<unsigned char> image, selectionOverlay;

	image.resize(noPixels * 3);
	selectionOverlay.resize(noPixels * 4);

	if (type == "SEQUENCE") {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;
				
				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImageIds) {
					const auto imageOffset	= displayImageId * noPixels;
					const auto pointId		= imageOffset + pixelId;

					pixelValue += pointsData.data[pointId];
				}
				
				pixelValue /= static_cast<float>(noDisplayImages);

				image[pixelId * 3 + 0] = pixelValue;
				image[pixelId * 3 + 1] = pixelValue;
				image[pixelId * 3 + 2] = pixelValue;
			}
		}
	}

	if (type == "STACK") {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImageIds) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				image[pixelId * 3 + 0] = pixelValue;
				image[pixelId * 3 + 1] = pixelValue;
				image[pixelId * 3 + 2] = pixelValue;
			}
		}

		if (_imageViewerPlugin->hasSelection()) {
			for (unsigned int index : _imageViewerPlugin->selection())
			{
				selectionOverlay[index * 4 + 0] = 0;
				selectionOverlay[index * 4 + 1] = 255;
				selectionOverlay[index * 4 + 2] = 0;
				selectionOverlay[index * 4 + 3] = 128;
			}
		}
	}

	_texture.setData(QOpenGLTexture::PixelFormat::RGB, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&image[0]));
	_selectionOverlayTexture.setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&selectionOverlay[0]));

	update();

	//qDebug() << _texture.isCreated();
	//qDebug() << _texture.isStorageAllocated();
}

void ImageViewerWidget::setupTextures(const QSize& imageSize)
{
	if (imageSize.width() == 0 || imageSize.height() == 0)
		return;
	
	qDebug() << "Setup texture: " << QString("%1x%2").arg(QString::number(imageSize.width()), QString::number(imageSize.height()));

	_texture.destroy();
	_texture.create();
	_texture.setSize(imageSize.width(), imageSize.height(), 1);
	_texture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	_texture.allocateStorage();

	_selectionOverlayTexture.destroy();
	_selectionOverlayTexture.create();
	_selectionOverlayTexture.setSize(imageSize.width(), imageSize.height(), 1);
	_selectionOverlayTexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	_selectionOverlayTexture.allocateStorage();
}

void ImageViewerWidget::drawQuad(const float& z)
{
	const auto imageSize		= _imageViewerPlugin->imageSize();
	const auto halfImageSize	= _imageViewerPlugin->imageSize() / 2;

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); glVertex3f(-halfImageSize.width(), -halfImageSize.height(), z);
		glTexCoord2f(0, 1); glVertex3f(-halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 1); glVertex3f(halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 0); glVertex3f(halfImageSize.width(), -halfImageSize.height(), z);
	}
	glEnd();
}

void ImageViewerWidget::drawTextureQuad(QOpenGLTexture& texture, const float& z)
{
	glEnable(GL_TEXTURE_2D);

	texture.bind();

	drawQuad(z);

	texture.release();

	glDisable(GL_TEXTURE_2D);
}

void ImageViewerWidget::initializeGL()
{
	initializeOpenGLFunctions();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";

	if (h == 0)
		h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);

	zoomExtents();
}

void ImageViewerWidget::paintGL()
{
	if (!_texture.isCreated())
		return;

	glClearColor(0.2, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScalef(_zoom, _zoom, 1);
	glTranslatef(_pan.x(), _pan.y(), 0);
	
	glDisable(GL_LIGHTING);

	drawTextureQuad(_texture, 0.5);
	drawTextureQuad(_selectionOverlayTexture, 0);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* event)
{
	_mousePosition = event->pos();
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
	qDebug() << "Moving mouse" << event->button();

	if (event->buttons() == Qt::LeftButton) {
		pan(event->pos().x() - _mousePosition.x(), -(event->pos().y() - _mousePosition.y()));
		_mousePosition = event->pos();
	}
}

void ImageViewerWidget::wheelEvent(QWheelEvent* event)
{
	qDebug() << "wheelEvent";

	const auto world_x = (event->posF().x() - _pan.x()) / _zoom;
	const auto world_y = (event->posF().y() - _pan.y()) / _zoom;
	
	auto zoomCenter = event->posF();

	zoomCenter.setY(height() - event->posF().y());

	if (event->delta() > 0) {
		zoomAt(zoomCenter, 1.f + _zoomSensitivity);
	}
	else {
		zoomAt(zoomCenter, 1.f - _zoomSensitivity);
	}
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu;

		QAction* zoomToExtentsAction = new QAction("Zoom extents", this);

		zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");
		
		connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);

		menu.addAction(zoomToExtentsAction);

		menu.addSeparator();
		menu.exec(mapToGlobal(event->pos()));
	}

	QOpenGLWidget::mouseReleaseEvent(event);
}

void ImageViewerWidget::pan(const float& dx, const float& dy) {
	qDebug() << "Pan";

	_pan.setX(_pan.x() + (dx / _zoom));
	_pan.setY(_pan.y() + (dy / _zoom));

	update();
}

void ImageViewerWidget::zoom(const float& factor) {
	qDebug() << "Zoom";

	_zoom *= factor;
	
	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);

	update();
}

void ImageViewerWidget::zoomAt(const QPointF& screenPosition, const float& factor)
{
	qDebug() << "Zoom at";

	pan(-screenPosition.x(), -screenPosition.y());
	zoom(factor);
	pan(screenPosition.x(), screenPosition.y());

	update();
}

void ImageViewerWidget::zoomExtents()
{
	if (_imageViewerPlugin->currentDataSetName().isEmpty())
		return;

	qDebug() << "Zoom extents";

	const auto imageSize = _imageViewerPlugin->imageSize();

	/*
	const auto world_x = (halfImageSize.width() - _pan.x()) / _zoom;
	const auto world_y = (halfImageSize.height() - _pan.y()) / _zoom;
	const auto screen_x = halfImageSize.width() * _zoom + _pan.x();
	const auto screen_y = halfImageSize.height() * _zoom + _pan.y();
	*/

	resetView();
	
	const auto factorX = (width() - _margin) / static_cast<float>(imageSize.width());
	const auto factorY = (height() - _margin) / static_cast<float>(imageSize.height());
	
	zoom(factorX < factorY ? factorX : factorY);
	pan(width() / 2, height() / 2);
}

void ImageViewerWidget::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);
	
	_zoom = 1.f;
}
