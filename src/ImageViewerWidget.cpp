#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"

#include <vector>
#include <set>

#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QList>
#include <QtMath>
#include <QGuiApplication>

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
	_selectionModifier(SelectionModifier::Replace),
	_selectionRealtime(false),
	_brushRadius(10.f),
	_brushRadiusDelta(1.f),
	_selectionColor(0, 1, 0),
	_selectionGeometryColor(1, 0, 0)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageIdsChanged, this, &ImageViewerWidget::onDisplayImageIdsChanged);
	connect(_imageViewerPlugin, QOverload<const QString&>::of(&ImageViewerPlugin::currentDataSetNameChanged), this, &ImageViewerWidget::onCurrentDataSetNameChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &ImageViewerWidget::onSelectedPointsChanged);

	createActions();
	createMenus();
}

ImageViewerWidget::SelectionType ImageViewerWidget::selectionType() const
{
	return _selectionType;
}

void ImageViewerWidget::setSelectionType(const SelectionType& selectionType)
{
	qDebug() << "Set selection type" << selectionType;

	_selectionType = selectionType;

	if (selectionType == SelectionType::Brush) {
		_selectionModifier = SelectionModifier::Add;
	}
	else {
		_selectionModifier = SelectionModifier::Replace;
	}

	update();

	emit selectionTypeChanged();
}

ImageViewerWidget::SelectionModifier ImageViewerWidget::selectionModifier() const
{
	return _selectionModifier;
}

void ImageViewerWidget::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	qDebug() << "Set selection modifier" << selectionModifier;

	if (selectionType() == SelectionType::Brush && selectionModifier == SelectionModifier::Replace) {
	}
	else {
		_selectionModifier = selectionModifier;

		emit selectionModifierChanged();
	}
}

void ImageViewerWidget::setBrushRadius(const float& brushRadius)
{
	_brushRadius = qBound(0.01f, 10000.f, brushRadius);

	update();

	emit brushRadiusChanged();
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
	
	std::vector<unsigned char> image;

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

	auto missed = QList<int>();

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

void ImageViewerWidget::onCurrentDataSetNameChanged()
{
	_selecting = false;

	zoomExtents();
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
	const auto halfImageSize = _imageViewerPlugin->imageSize() / 2;

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); glVertex3f(-halfImageSize.width(), -halfImageSize.height(), z);
		glTexCoord2f(0, 1); glVertex3f(-halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 1); glVertex3f(halfImageSize.width(), halfImageSize.height(), z);
		glTexCoord2f(1, 0); glVertex3f(halfImageSize.width(), -halfImageSize.height(), z);
	}
	glEnd();
}

void ImageViewerWidget::drawCircle(const QPointF& center, const float& radius, const int& noSegments /*= 30*/)
{
	glBegin(GL_LINE_LOOP);
	
	for (int ii = 0; ii < noSegments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(noSegments);
		float x = radius * cosf(theta);
		float y = radius * sinf(theta);
		
		glVertex2f(x + center.x(), y + center.y());
	}

	glEnd();
}

void ImageViewerWidget::drawSelectionRectangle(const QPoint& start, const QPoint& end)
{
	const auto z = -0.5;

	glColor4f(_selectionGeometryColor.red(), _selectionGeometryColor.green(), _selectionGeometryColor.blue(), 1.f);

	glBegin(GL_LINE_LOOP);

	glVertex3f(start.x(), height() - start.y(), z);
	glVertex3f(end.x(), height() - start.y(), z);
	glVertex3f(end.x(), height() - end.y(), z);
	glVertex3f(start.x(), height() - end.y(), z);

	glEnd();
}

void ImageViewerWidget::drawSelectionBrush()
{
	auto brushCenter = QWidget::mapFromGlobal(QCursor::pos());

	brushCenter.setY(height() - brushCenter.y());

	glColor4f(_selectionGeometryColor.red(), _selectionGeometryColor.green(), _selectionGeometryColor.blue(), 1.f);

	drawCircle(brushCenter, _brushRadius, 20);
}

void ImageViewerWidget::drawTextureQuad(QOpenGLTexture& texture, const float& z)
{
	glEnable(GL_TEXTURE_2D);

	texture.bind();

	drawQuad(z);

	texture.release();

	glDisable(GL_TEXTURE_2D);
}

void ImageViewerWidget::drawSelectionGeometry()
{
	switch (_selectionType)
	{
		case SelectionType::Rectangle:
		{
			if (_selecting) {
				const auto currentMouseWorldPos = QWidget::mapFromGlobal(QCursor::pos());
				drawSelectionRectangle(_initialMousePosition, currentMouseWorldPos);
			}
			
			break;
		}

		case SelectionType::Brush:
		{
			drawSelectionBrush();
			break;
		}

		default:
			break;
	}
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

	if (_imageViewerPlugin->imageCollectionType() == "STACK") {
		drawTextureQuad(_selectionOverlayTexture, 0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawSelectionGeometry();
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* mouseEvent) 
{
	if (!imageInitialized())
		return;

	//qDebug() << "Mouse press event" << event->pos();

	if (mouseEvent->button() == Qt::LeftButton) {
		_mousePosition = mouseEvent->pos();

		if (mouseEvent->modifiers() & Qt::AltModifier) {

		}
		else {
			if (_imageViewerPlugin->isStack()) {
				/*
				if (_selectionModifier == SelectionModifier::Replace) {
					qDebug() << "Reset selection";

					_imageViewerPlugin->setSelection(Indices());
				}
				*/

				_initialMousePosition = _mousePosition;
				_selecting = true;
			}
		}
	}
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	if (!imageInitialized())
		return;

	//qDebug() << "Mouse move event" << event->pos();

	if (mouseEvent->buttons() == Qt::LeftButton) {
		if (mouseEvent->modifiers() & Qt::AltModifier) {
			pan(QPointF(mouseEvent->pos().x() - _mousePosition.x(), -(mouseEvent->pos().y() - _mousePosition.y())));
		}
		else {
			if (_imageViewerPlugin->isStack()) {
				_selecting = true;

				if (_selectionRealtime || _selectionType == SelectionType::Brush) {
					updateSelection();
				}
			}
		}

		_mousePosition = mouseEvent->pos();

		update();
	}
	else {
		if (_selectionType == SelectionType::Brush) 
			update();
	}
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!imageInitialized())
		return;

	//qDebug() << "Mouse wheel event" << event->delta();

	if (wheelEvent->modifiers() & Qt::AltModifier) {
		const auto world_x = (wheelEvent->posF().x() - _pan.x()) / _zoom;
		const auto world_y = (wheelEvent->posF().y() - _pan.y()) / _zoom;

		auto zoomCenter = wheelEvent->posF();

		zoomCenter.setY(height() - wheelEvent->posF().y());

		if (wheelEvent->delta() > 0) {
			zoomAt(zoomCenter, 1.f - _zoomSensitivity);
		}
		else {
			zoomAt(zoomCenter, 1.f + _zoomSensitivity);
		}

		update();
	}
	else {
		if (_selectionType == SelectionType::Brush) {
			if (wheelEvent->delta() > 0) {
				setBrushRadius(_brushRadius + _brushRadiusDelta);
			}
			else {
				setBrushRadius(_brushRadius - _brushRadiusDelta);
			}
		}
	}
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent) {

	if (!imageInitialized())
		return;

	//qDebug() << "Mouse release event";

	if (mouseEvent->button() == Qt::RightButton)
	{
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	if (mouseEvent->modifiers() & Qt::AltModifier) {

	}
	else {
		if (_selecting) {
			if (_imageViewerPlugin->isStack()) {
				_selecting = false;

				updateSelection();
			}
		}
	}

	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
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
	return QPoint(((screen.x()) / _zoom) - _pan.x(), -((-(screen.y() - height()) / _zoom) - _pan.y()));
}

QPoint ImageViewerWidget::worldToScreen(const QPoint& world) const
{
	return QPoint();
}

void ImageViewerWidget::updateSelection()
{
	qDebug() << "Update selection" << _selectionType;

	const auto imageSize		= _imageViewerPlugin->imageSize();
	const auto halfImageSize	= _imageViewerPlugin->imageSize() / 2;
	const auto imageRect		= QRect(-halfImageSize.width(), -halfImageSize.height(), imageSize.width(), imageSize.height());

	switch (_selectionType)
	{
		case SelectionType::Rectangle: {
			const auto initialMouseWorldPos = screenToWorld(QPoint(_initialMousePosition.x(), _initialMousePosition.y()));
			const auto currentMouseWorldPos = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto selectionTopLeft		= QPoint(qMin(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMin(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionBottomRight = QPoint(qMax(initialMouseWorldPos.x(), currentMouseWorldPos.x()), qMax(initialMouseWorldPos.y(), currentMouseWorldPos.y()));
			const auto selectionRect		= QRect(selectionTopLeft, selectionBottomRight);

			//qDebug() << imageRect << selectionRect;

			if (imageRect.intersects(selectionRect)) {
				auto imageSelection	= selectionRect.intersected(imageRect);
				const auto noSelectedPixels = imageSelection.width() * imageSelection.height();
				
				auto selection = Indices();

				selection.resize(noSelectedPixels);

				const auto imageWidth			= imageSize.width();
				const auto imageHeight			= imageSize.height();
				const auto imageSelectionWidth	= imageSelection.width();
				const auto imageSelectionHeight	= imageSelection.height();

				auto selectionIndex = 0;
				
				for (int x = imageSelection.x(); x < (imageSelection.x() + imageSelection.width()); x++) {
					for (int y = imageSelection.y(); y < (imageSelection.y() + imageSelection.height()); y++) {
						
						const auto imageY = imageHeight - (y + (imageHeight / 2)) - 1;
						selection[selectionIndex] = imageY * imageWidth + (x + (imageWidth / 2));

						selectionIndex++;
					}
				}

				select(selection);
			}

			break;
		}

		case SelectionType::Brush: {
			const auto currentMouseWorldPos = screenToWorld(QPoint(_mousePosition.x(), _mousePosition.y()));
			const auto brushRadius			= _brushRadius / _zoom;
			const auto offset				= QPoint(qCeil(brushRadius), qCeil(brushRadius));
			const auto selectionRect		= QRect(currentMouseWorldPos - offset, currentMouseWorldPos + offset);

			if (imageRect.intersects(selectionRect)) {
				//qDebug() << "Intersects";

				auto imageSelection = selectionRect.intersected(imageRect);

				const auto noSelectedPixels = imageSelection.width() * imageSelection.height();

				auto selection = Indices();

				selection.reserve(noSelectedPixels);

				const auto imageWidth			= imageSize.width();
				const auto imageHeight			= imageSize.height();
				const auto imageSelectionWidth	= imageSelection.width();
				const auto imageSelectionHeight = imageSelection.height();
				const auto center				= QVector2D(currentMouseWorldPos);

				for (int x = imageSelection.x(); x < (imageSelection.x() + imageSelection.width()); x++) {
					for (int y = imageSelection.y(); y < (imageSelection.y() + imageSelection.height()); y++) {
						const auto imageY = imageHeight - (y + (imageHeight / 2)) - 1;

						const auto pos = QVector2D(x, y) - QVector2D(selectionRect.center());
						//qDebug() << pos.length();

						if (pos.length() < (_brushRadius / _zoom)) {
							selection.push_back(imageY * imageWidth + (x + (imageWidth / 2)));
						}
					}
				}

				select(selection);
			}

			break;
		}

		default:
			break;
	}
}

void ImageViewerWidget::select(std::vector<unsigned int>& indices)
{
	if (indices.size() > 0) {
		switch (_selectionModifier) {
			case SelectionModifier::Replace:
			{
				//qDebug() << "Replace selection";

				_imageViewerPlugin->setSelection(indices);
				break;
			}

			
			case SelectionModifier::Add:
			{
				//qDebug() << "Add to selection";

				const auto selection = _imageViewerPlugin->selection();
				auto selectionSet = std::set<Index>(selection.begin(), selection.end());

				for (Index index : indices) {
					selectionSet.insert(index);
				}

				_imageViewerPlugin->setSelection(Indices(selectionSet.begin(), selectionSet.end()));
				
				break;
			}

			case SelectionModifier::Remove:
			{
				//qDebug() << "Remove from selection";

				const auto selection = _imageViewerPlugin->selection();
				auto selectionSet = std::set<Index>(selection.begin(), selection.end());

				for (Index index : indices) {
					selectionSet.erase(index);
				}

				_imageViewerPlugin->setSelection(Indices(selectionSet.begin(), selectionSet.end()));

				break;
			}
		}
	}
}

void ImageViewerWidget::createActions()
{
	_zoomToExtentsAction = new QAction("Zoom extents", this);
	_zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");

	connect(_zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);

	_rectangleSelectionAction = new QAction("Rectangle", this);

	_brushSelectionAction = new QAction("Brush", this);
	_brushSelectionAction->setEnabled(false);

	_freehandSelectionAction = new QAction("Freehand", this);
	_freehandSelectionAction->setEnabled(false);

	_clearSelectionAction = new QAction("Clear", this);
}

void ImageViewerWidget::createMenus()
{
	_contextMenu = new QMenu();

	

	_selectionMenu = new QMenu("Selection");

	_selectionMenu->addAction(_rectangleSelectionAction);
	_selectionMenu->addAction(_brushSelectionAction);
	_selectionMenu->addAction(_freehandSelectionAction);
	_selectionMenu->addSeparator();
	_selectionMenu->addAction(_clearSelectionAction);

	
}

QMenu* ImageViewerWidget::contextMenu() const
{
	_contextMenu->addAction(_zoomToExtentsAction);

	if (_imageViewerPlugin->imageCollectionType() == "STACK") {
		_contextMenu->addSeparator();
		_contextMenu->addMenu(_selectionMenu);
	}

	return _contextMenu;
}
