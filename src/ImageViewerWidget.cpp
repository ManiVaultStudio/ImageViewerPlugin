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
	_initialMousePosition(),
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selecting(false),
	_selectionType(SelectionType::Rectangle),
	_selectionRealtime(false)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageIdsChanged, this, &ImageViewerWidget::onDisplayImageIdsChanged);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDataSetNameChanged), this, &ImageViewerWidget::zoomExtents);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &ImageViewerWidget::onSelectedPointsChanged);
}

void ImageViewerWidget::onDisplayImageIdsChanged()
{
	const auto imageSize			= _imageViewerPlugin->imageSize();
	const auto noPixels				= imageSize.width() * imageSize.height();
	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto displayImageIds		= _imageViewerPlugin->displayImageIds();
	const auto noDisplayImages		= displayImageIds.size();
	const auto noImages				= _imageViewerPlugin->noImages();
	const auto width				= imageSize.width();
	const auto height				= imageSize.height();
	
	if (QSize(_texture.width(), _texture.height()) != imageSize) {
		setupTextures(imageSize);
	}

	PointsPlugin& pointsData = _imageViewerPlugin->pointsData();
	
	std::vector<unsigned char> image, selectionOverlay;

	image.resize(noPixels * 3);

	if (imageCollectionType == "SEQUENCE") {
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

	if (imageCollectionType == "STACK") {
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

	update();
}

void ImageViewerWidget::onSelectedPointsChanged()
{
	qDebug() << "Selected points changed";

	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto imageSize			= _imageViewerPlugin->imageSize();
	const auto noPixels				= imageSize.width() * imageSize.height();

	std::vector<unsigned char> selectionOverlay;
	
	selectionOverlay.resize(noPixels * 4);

	if (imageCollectionType == "STACK") {
		if (_imageViewerPlugin->hasSelection()) {
			for (unsigned int index : _imageViewerPlugin->selection())
			{
				selectionOverlay[index * 4 + 0] = 0;
				selectionOverlay[index * 4 + 1] = 255;
				selectionOverlay[index * 4 + 2] = 0;
				selectionOverlay[index * 4 + 3] = 128;
			}
		}

		_selectionOverlayTexture.setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&selectionOverlay[0]));
	}

	update();
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

void ImageViewerWidget::drawQuad(const float& z) {
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

void ImageViewerWidget::drawSelectionRectangle(const QPoint& start, const QPoint& end) {
	
	const auto z = -0.5;

	glColor4f(0.f, 1.f, 0.f, 0.1f);

	glBegin(GL_QUADS);
	{
		glVertex3f(start.x(), height() - start.y(), z);
		glVertex3f(end.x(), height() - start.y(), z);
		glVertex3f(end.x(), height() - end.y(), z);
		glVertex3f(start.x(), height() - end.y(), z);
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
	qDebug() << "Initializing OpenGL";

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
	glOrtho(0, w, 0, h, -100, 100);

	zoomExtents();
}

void ImageViewerWidget::paintGL() {

	if (!imageInitialized())
		return;

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	glScalef(_zoom, _zoom, 1);
	glTranslatef(_pan.x(), _pan.y(), 0);
	
	glColor4f(1.f, 1.f, 1.f, 1.f);

	drawTextureQuad(_texture, 0.5);
	drawTextureQuad(_selectionOverlayTexture, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (_selecting) {
		const auto currentMousePosition = QWidget::mapFromGlobal(QCursor::pos());
		drawSelectionRectangle(_initialMousePosition, currentMousePosition);
	}
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* event) 
{
	if (!imageInitialized())
		return;

	qDebug() << "Mouse press event" << event->pos();

	_mousePosition = event->pos();

	if (event->modifiers() & Qt::AltModifier) {

	}
	else {
		if (_imageViewerPlugin->isStack()) {
			_initialMousePosition = _mousePosition;
			_selecting = true;
		}
	}
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* event) {

	if (!imageInitialized())
		return;

	qDebug() << "Mouse move event" << event->pos();

	if (event->buttons() == Qt::LeftButton) {
		if (event->modifiers() & Qt::AltModifier) {
			pan(QPointF(event->pos().x() - _mousePosition.x(), -(event->pos().y() - _mousePosition.y())));
		}
		else {
			if (_imageViewerPlugin->isStack()) {
				_selecting = true;

				if (_selectionRealtime) {
					updateSelection();
				}
			}
		}

		_mousePosition = event->pos();

		update();
	}
}

void ImageViewerWidget::wheelEvent(QWheelEvent* event) {

	if (!imageInitialized())
		return;

	qDebug() << "Mouse wheel event" << event->delta();

	if (event->modifiers() & Qt::AltModifier) {
		const auto world_x = (event->posF().x() - _pan.x()) / _zoom;
		const auto world_y = (event->posF().y() - _pan.y()) / _zoom;

		auto zoomCenter = event->posF();

		zoomCenter.setY(height() - event->posF().y());

		if (event->delta() > 0) {
			zoomAt(zoomCenter, 1.f - _zoomSensitivity);
		}
		else {
			zoomAt(zoomCenter, 1.f + _zoomSensitivity);
		}

		update();
	}
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* event) {

	if (!imageInitialized())
		return;

	qDebug() << "Mouse release event";

	if (event->button() == Qt::RightButton)
	{
		QMenu menu;

		QAction* zoomToExtentsAction = new QAction("Zoom extents", this);

		// zoomToExtentsAction->setShortcut(QKeySequence(Qt::Key_Space));
		zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");
		
		connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);

		menu.addAction(zoomToExtentsAction);

		menu.addSeparator();
		menu.exec(mapToGlobal(event->pos()));
	}

	if (event->modifiers() & Qt::AltModifier) {

	}
	else {
		if (_imageViewerPlugin->isStack()) {
			_selecting = false;

			updateSelection();
		}
	}

	update();

	QOpenGLWidget::mouseReleaseEvent(event);
}

void ImageViewerWidget::pan(const QPointF& delta) {

	qDebug() << "Pan" << delta;

	_pan.setX(_pan.x() + (delta.x() / _zoom));
	_pan.setY(_pan.y() + (delta.y() / _zoom));
}

void ImageViewerWidget::zoom(const float& factor) {

	qDebug() << "Zoom" << factor;

	_zoom *= factor;
	
	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);
}

void ImageViewerWidget::zoomAt(const QPointF& screenPosition, const float& factor) {

	qDebug() << "Zoom at" << screenPosition << factor;

	pan(QPointF(-screenPosition.x(), -screenPosition.y()));
	zoom(factor);
	pan(QPointF(screenPosition.x(), screenPosition.y()));
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
	pan(QPointF(width() / 2, height() / 2));

	update();
}

void ImageViewerWidget::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);
	
	_zoom = 1.f;

	update();
}

bool ImageViewerWidget::imageInitialized() const
{
	return _texture.isCreated();
}

QPoint ImageViewerWidget::screenToWorld(const QPoint& screen) const
{
	const auto halfSize = size() / 2;
	
	return QPoint((screen.x() - halfSize.width()) / _zoom, (screen.y() - halfSize.height()) / _zoom);
}

QPoint ImageViewerWidget::worldToScreen(const QPoint& world) const
{
	return QPoint();
}

void ImageViewerWidget::updateSelection()
{
	qDebug() << "Update selection";

	switch (_selectionType)
	{
		case SelectionType::Rectangle: {
			const auto initialMousePosition = screenToWorld(QPoint(_initialMousePosition.x(), _initialMousePosition.y()));
			const auto currentMousePosition = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto selectionTopLeft = QPoint(qMin(initialMousePosition.x(), currentMousePosition.x()), qMin(initialMousePosition.y(), currentMousePosition.y()));
			const auto selectionBottomRight = QPoint(qMax(initialMousePosition.x(), currentMousePosition.x()), qMax(initialMousePosition.y(), currentMousePosition.y()));
			const auto selectionRect = QRect(selectionTopLeft, selectionBottomRight);
			const auto halfImageSize = _imageViewerPlugin->imageSize() / 2;
			const auto imageTopLeft = QPoint(-halfImageSize.width(), -halfImageSize.height());
			const auto imageBottomRight = QPoint(halfImageSize.width(), halfImageSize.height());
			const auto imageRect = QRect(imageTopLeft, imageBottomRight);

			if (imageRect.intersects(selectionRect)) {
				const auto roi = selectionRect.intersected(imageRect);

				/*
				for (int x = 0; x < width; x++) {
					for (int y = 0; y < height; y++) {
					}
				}
				*/
			}

			break;
		}

		default:
			break;
	}
}