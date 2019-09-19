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
#include <QPainter>
#include <QGuiApplication>

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	_imageViewerPlugin(imageViewerPlugin),
	_textureDataMap(),
	_textureMap(),
	_interactionMode(InteractionMode::Selection),
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
	_brushRadiusDelta(2.0f),
	_selectionColor(255, 0, 0, 200),
	_selectionProxyColor(245, 184, 17, 100),
	_selectionGeometryColor(255, 0, 0, 255),
	_selection(),
	_zoomToExtentsAction(nullptr),
	_rectangleSelectionAction(nullptr),
	_brushSelectionAction(nullptr),
	_freehandSelectionAction(nullptr),
	_clearSelectionAction(nullptr),
	_contextMenu(nullptr),
	_selectionMenu(nullptr)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImagesChanged, this, &ImageViewerWidget::onDisplayImagesChanged);
	//connect(_imageViewerPlugin, &ImageViewerPlugin::displayImagesChanged, this, &ImageViewerWidget::onDisplayImagesChanged);
	
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	
	/*connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &ImageViewerWidget::onSelectedPointsChanged);
	*/

	createActions();
	createMenus();

	QSurfaceFormat surfaceFormat;
	
	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	//surfaceFormat.setVersion(4, 1);
	surfaceFormat.setSamples(16);
	
	setFormat(surfaceFormat);

	_textureDataMap.insert(std::make_pair("image", TextureData()));
	_textureDataMap.insert(std::make_pair("overlay", TextureData()));
	_textureDataMap.insert(std::make_pair("selection", TextureData()));

	_textureMap.emplace("image", QOpenGLTexture::Target2D);
	_textureMap.emplace("overlay", QOpenGLTexture::Target2D);
	_textureMap.emplace("selection", QOpenGLTexture::Target2D);
}

ImageViewerWidget::InteractionMode ImageViewerWidget::interactionMode() const
{
	return _interactionMode;
}

void ImageViewerWidget::setInteractionMode(const InteractionMode& interactionMode)
{
	qDebug() << "Set interaction mode" << interactionMode;

	switch (interactionMode)
	{
		case InteractionMode::Navigation:
			QWidget::setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
			QWidget::setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}

	_interactionMode = interactionMode;
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
	qDebug() << "Set brush radius" << brushRadius;

	_brushRadius = qBound(0.01f, 10000.f, brushRadius);

	update();

	emit brushRadiusChanged();
}

void ImageViewerWidget::onDisplayImagesChanged(const Indices& displayImages)
{
	if (!isValid())
		return;

	const auto imageSize			= _imageViewerPlugin->imageSize();
	const auto noPixels				= _imageViewerPlugin->noPixels();
	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto noDisplayImages		= displayImages.size();
	const auto noImages				= _imageViewerPlugin->noImages();
	const auto width				= imageSize.width();
	const auto height				= imageSize.height();
	
	if (QSize(texture("image").width(), texture("image").height()) != imageSize) {
		setupTextures();
	}

	auto& pointsData		= _imageViewerPlugin->pointsData();
	auto& imageTextureData	= textureData("image");

	if (imageCollectionType == ImageCollectionType::Sequence) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;
				
				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto imageOffset	= displayImageId * noPixels;
					const auto pointId		= imageOffset + pixelId;

					pixelValue += pointsData.data[pointId];
				}
				
				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}

	if (imageCollectionType == ImageCollectionType::Stack) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}

	if (imageCollectionType == ImageCollectionType::MultiPartSequence) {
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				const auto pixelId = y * width + x;

				float pixelValue = 0.f;

				for (unsigned int displayImageId : displayImages) {
					const auto pointId = (pixelId * noImages) + displayImageId;

					pixelValue += pointsData.data[pointId];
				}

				pixelValue /= static_cast<float>(noDisplayImages);

				const auto offset = pixelId * 4;

				imageTextureData[offset + 0] = pixelValue;
				imageTextureData[offset + 1] = pixelValue;
				imageTextureData[offset + 2] = pixelValue;
				imageTextureData[offset + 3] = 255;
			}
		}
	}

	applyTextureData("image");

	update();
}

void ImageViewerWidget::onSelectedPointsChanged()
{
	qDebug() << "Selected points changed";

	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto imageSize			= _imageViewerPlugin->imageSize();
	const auto noPixels				= imageSize.width() * imageSize.height();

	auto missed = QList<int>();

	resetTextureData("selection");
	resetTexture("overlay");

	TextureData& selectionTextureData = textureData("selection");

	if (imageCollectionType == ImageCollectionType::Stack) {
		if (_imageViewerPlugin->hasSelection()) {
			for (unsigned int index : _imageViewerPlugin->selection())
			{
				const auto offset = index * 4;

				selectionTextureData[offset + 0] = _selectionColor.red();
				selectionTextureData[offset + 1] = _selectionColor.green();
				selectionTextureData[offset + 2] = _selectionColor.blue();
				selectionTextureData[offset + 3] = _selectionColor.alpha();
			}
		}

		applyTextureData("selection");
	}

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	_selecting = false;

	resetTexture("overlay");

	zoomExtents();
}

void ImageViewerWidget::setupTexture(QOpenGLTexture& texture)
{
	texture.destroy();
	texture.create();
	texture.setSize(_imageViewerPlugin->imageSize().width(), _imageViewerPlugin->imageSize().height(), 1);
	texture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	texture.allocateStorage();
}

void ImageViewerWidget::setupTextures()
{
	if (_imageViewerPlugin->noPixels() == 0)
		return;
	
	qDebug() << "Setup textures";

	resetTextureData("image");
	resetTextureData("overlay");
	resetTextureData("selection");

	setupTexture(texture("image"));
	setupTexture(texture("overlay"));
	setupTexture(texture("selection"));
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

void ImageViewerWidget::drawInfo(QPainter* painter)
{
	QFont font = painter->font();
	font.setFamily("courier");
	font.setPixelSize(16);
	
	painter->setFont(font);

	const QRect rectangle = QRect(0, 0, 1000, 500);
	QRect boundingRect;

	auto infoLines = QStringList();

	infoLines << "ALT: Navigation mode";

	if (QGuiApplication::queryKeyboardModifiers().testFlag(Qt::AltModifier))
	{
		infoLines << "ALT + LMB: Pan";
		infoLines << "ALT + MWL: Zoom";
	}
	else {
		infoLines << "R: Rectangle selection";
		infoLines << "B: Brush selection";
	}

	painter->setPen(Qt::white);
	painter->drawText(rectangle, 0, infoLines.join("\n"), &boundingRect);
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);
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

	glClearColor(0.1, 0.1, 0.1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!imageInitialized())
		return;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	glScalef(_zoom, _zoom, 1);
	glTranslatef(_pan.x(), _pan.y(), 0);
	
	glColor4f(1.f, 1.f, 1.f, 1.f);

	drawTextureQuad(texture("image"), 1.0f);

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		drawTextureQuad(texture("overlay"), 0.5f);
		drawTextureQuad(texture("selection"), 0.0f);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (_interactionMode == InteractionMode::Selection) {
		drawSelectionGeometry();
	}
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
			if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
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
		if (_interactionMode == InteractionMode::Navigation) {
			QWidget::setCursor(Qt::ClosedHandCursor);
		}

		if (mouseEvent->modifiers() & Qt::AltModifier) {
			pan(QPointF(mouseEvent->pos().x() - _mousePosition.x(), -(mouseEvent->pos().y() - _mousePosition.y())));
		}
		else {
			if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
				_selecting = true;
				updateSelection();
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

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent) {

	if (!imageInitialized())
		return;

	//qDebug() << "Mouse release event";

	if (_interactionMode == InteractionMode::Navigation) {
		QWidget::setCursor(Qt::OpenHandCursor);
	}

	if (mouseEvent->button() == Qt::RightButton)
	{
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	if (mouseEvent->modifiers() & Qt::AltModifier) {

	}
	else {
		if (_selecting) {
			if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
				_selecting = false;

				updateSelection();
			}

			commitSelection();
		}
	}

	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
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
	if (_imageViewerPlugin->currentDataset().isEmpty())
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

bool ImageViewerWidget::imageInitialized()
{
	return texture("image").isCreated();
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
	//qDebug() << "Update selection" << _selectionType;

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

				modifySelection(selection);
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

				modifySelection(selection);
			}

			break;
		}

		default:
			break;
	}
}

void ImageViewerWidget::modifySelection(Indices& indices)
{
	if (indices.size() > 0) {
		switch (_selectionModifier) {
			case SelectionModifier::Replace:
			{
				//qDebug() << "Replace selection";
				
				_selection = indices;

				break;
			}

			
			case SelectionModifier::Add:
			{
				//qDebug() << "Add to selection";

				auto selectionSet = std::set<unsigned int>(_selection.begin(), _selection.end());

				for (Index index : indices) {
					selectionSet.insert(index);
				}

				_selection = Indices(selectionSet.begin(), selectionSet.end());
				
				break;
			}

			case SelectionModifier::Remove:
			{
				//qDebug() << "Remove from selection";

				auto selectionSet = std::set<unsigned int>(_selection.begin(), _selection.end());

				for (Index index : indices) {
					selectionSet.erase(index);
				}

				_selection = Indices(selectionSet.begin(), selectionSet.end());

				break;
			}
		}

		resetTextureData("overlay");

		TextureData& overlayTextureData = textureData("overlay");

		for (Index index : _selection) {
			const auto offset = index * 4;

			overlayTextureData[offset + 0] = _selectionProxyColor.red();
			overlayTextureData[offset + 1] = _selectionProxyColor.green();
			overlayTextureData[offset + 2] = _selectionProxyColor.blue();
			overlayTextureData[offset + 3] = _selectionProxyColor.alpha();
		}

		applyTextureData("overlay");

		update();
	}
}

void ImageViewerWidget::commitSelection()
{
	qDebug() << "Comitting selection to core";

	resetTextureData("overlay");
	
	_imageViewerPlugin->setSelection(_selection);
}

void ImageViewerWidget::resetTexture(const QString & textureName)
{
	if (texture(textureName).isCreated()) {
		resetTextureData(textureName);
		applyTextureData(textureName);
	}
}

void ImageViewerWidget::resetTextureData(const QString& textureName)
{
	TextureData& data = textureData(textureName);

	data.resize(_imageViewerPlugin->noPixels() * 4);

	std::fill(data.begin(), data.end(), 0);
}

void ImageViewerWidget::applyTextureData(const QString& name)
{
	texture(name).setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&textureData(name)[0]));
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

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		_contextMenu->addSeparator();
		_contextMenu->addMenu(_selectionMenu);
	}

	return _contextMenu;
}

QOpenGLTexture& ImageViewerWidget::texture(const QString& name)
{
	return _textureMap.at(name);
}

TextureData& ImageViewerWidget::textureData(const QString& textureName)
{
	return _textureDataMap.at(textureName);
}
