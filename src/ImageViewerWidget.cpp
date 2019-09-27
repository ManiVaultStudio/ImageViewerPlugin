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
	_selectedPixelCoordinates(),
	_zoomToExtentsAction(nullptr),
	_imageSize()
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewerWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectedPointsChanged, this, &ImageViewerWidget::onSelectedPointsChanged);
	
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

void ImageViewerWidget::onDisplayImageChanged(const QSize& imageSize, const TextureData& displayImage)
{
	if (!isValid())
		return;

	qDebug() << "Display image changed";

	auto shouldZoomExtents = false;

	if (imageSize != _imageSize) {
		_imageSize = imageSize;

		setupTextures();

		shouldZoomExtents = true;
	}
	
	textureData("image") = displayImage;

	applyTextureData("image");

	update();

	if (shouldZoomExtents)
		zoomExtents();
}

void ImageViewerWidget::onSelectedPointsChanged()
{
	qDebug() << "Selected points changed";

	const auto imageCollectionType	= _imageViewerPlugin->imageCollectionType();
	const auto noPixels				= _imageSize.width() * _imageSize.height();

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

			//_selectedPixelCoordinates = static_cast<Indices>(_imageViewerPlugin->selection());
		}

		applyTextureData("selection");
	}

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	_selecting = false;

	resetTexture("overlay");
}

void ImageViewerWidget::drawQuad(const float& z) {
	const auto halfImageSize = _imageSize / 2;

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

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack || _imageViewerPlugin->imageCollectionType() == ImageCollectionType::MultiPartSequence) {
		if (mouseEvent->button() == Qt::RightButton)
		{
			contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
		}
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

	resetView();
	
	const auto factorX = (width() - _margin) / static_cast<float>(_imageSize.width());
	const auto factorY = (height() - _margin) / static_cast<float>(_imageSize.height());
	
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

	const auto halfImageSize	= _imageSize / 2;
	const auto imageRect		= QRect(-halfImageSize.width(), -halfImageSize.height(), _imageSize.width(), _imageSize.height());

	resetTextureData("overlay");

	auto& overlayTextureData = textureData("overlay");

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
				const auto imageSelection		= selectionRect.intersected(imageRect);
				const auto noSelectedPixels		= imageSelection.width() * imageSelection.height();
				
				auto selectedPixelCoordinates = PixelCoordinates();

				selectedPixelCoordinates.reserve(noSelectedPixels);

				auto selectionIndex = 0;
				
				const auto left		= imageSelection.x() + halfImageSize.width();
				const auto right	= (imageSelection.x() + imageSelection.width()) + halfImageSize.width();
				const auto top		= imageSelection.y() + halfImageSize.height();
				const auto bottom	= (imageSelection.y() + imageSelection.height()) + halfImageSize.height();

				for (std::int32_t x = left; x < right; x++) {
					for (std::int32_t y = top; y < bottom; y++) {
						const auto pixelCoordinate = PixelCoordinate(x, _imageSize.height() - y);

						selectedPixelCoordinates.push_back(pixelCoordinate);
					}
				}

				applyTextureData("overlay");
				modifySelection(selectedPixelCoordinates);
			}

			break;
		}

		/*
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

				const auto imageWidth			= _imageSize.width();
				const auto imageHeight			= _imageSize.height();
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
		*/

		default:
			break;
	}
}

void ImageViewerWidget::modifySelection(const PixelCoordinates& selectedPixelCoordinates)
{
	qDebug() << "Modify selection";

	if (selectedPixelCoordinates.size() > 0) {
		switch (_selectionModifier) {
			case SelectionModifier::Replace:
			{
				//qDebug() << "Replace selection";
				
				_selectedPixelCoordinates = selectedPixelCoordinates;

				break;
			}
			
			case SelectionModifier::Add:
			{
				//qDebug() << "Add to selection";

				auto selectionSet = std::set<PixelCoordinate>(_selectedPixelCoordinates.begin(), _selectedPixelCoordinates.end());

				for (PixelCoordinate pixelCoordinate : selectedPixelCoordinates) {
					selectionSet.insert(pixelCoordinate);
				}

				_selectedPixelCoordinates = PixelCoordinates(selectionSet.begin(), selectionSet.end());
				
				break;
			}

			case SelectionModifier::Remove:
			{
				//qDebug() << "Remove from selection";

				auto selectionSet = std::set<PixelCoordinate>(_selectedPixelCoordinates.begin(), _selectedPixelCoordinates.end());

				for (PixelCoordinate pixelCoordinate : selectedPixelCoordinates) {
					selectionSet.erase(pixelCoordinate);
				}

				_selectedPixelCoordinates = PixelCoordinates(selectionSet.begin(), selectionSet.end());

				break;
			}
		}
	} 
	else
	{
		_selectedPixelCoordinates = PixelCoordinates();
	}

	resetTextureData("overlay");

	TextureData& overlayTextureData = textureData("overlay");

	for (PixelCoordinate pixelCoordinate : _selectedPixelCoordinates) {
		const auto offset = ImageViewerPlugin::pixelBufferOffset(_imageSize, pixelCoordinate.first, pixelCoordinate.second);

		overlayTextureData[offset + 0] = _selectionProxyColor.red();
		overlayTextureData[offset + 1] = _selectionProxyColor.green();
		overlayTextureData[offset + 2] = _selectionProxyColor.blue();
		overlayTextureData[offset + 3] = _selectionProxyColor.alpha();
	}

	applyTextureData("overlay");

	update();
}

void ImageViewerWidget::clearSelection()
{
	qDebug() << "Clear selection";

	modifySelection(PixelCoordinates());
	commitSelection();
}

void ImageViewerWidget::commitSelection()
{
	qDebug() << "Commit selection to core";

	resetTextureData("overlay");
	
	/*
	for (std::int32_t d = 0; d < noDimensions; d++) {
		switch (_imageViewerPlugin->imageCollectionType())
		{
		case ImageCollectionType::Stack:
		{
			const auto pointId = ImageViewerPlugin::stackCoordinateToPointId(_imageSize, noDimensions, d, imageX, imageY);
			selection[selectionIndex] = pointId;
			break;
		}

		default:
			break;
		}

		selectionIndex++;
	}
	*/

	// _imageViewerPlugin->setSelection(_selectedPixelCoordinates);
}

void ImageViewerWidget::applyTextureData(const QString& name)
{
	texture(name).setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt8, static_cast<void*>(&textureData(name)[0]));
}

QMenu* ImageViewerWidget::contextMenu()
{
	auto* contextMenu = new QMenu();

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		contextMenu->addMenu(viewMenu());
		contextMenu->addSeparator();
		contextMenu->addMenu(selectionMenu());
	}

	return contextMenu;
}

QMenu* ImageViewerWidget::viewMenu()
{
	auto* viewMenu = new QMenu("View");

	auto* zoomToExtentsAction = new QAction("Zoom extents");
	
	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);

	viewMenu->addAction(zoomToExtentsAction);

	return viewMenu;
}

QMenu* ImageViewerWidget::selectionMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction	= new QAction("Rectangle");
	auto* brushSelectionAction		= new QAction("Brush");
	auto* freehandSelectionAction	= new QAction("Freehand", this);
	auto* clearSelectionAction		= new QAction("Clear");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle);  });
	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush);  });
	connect(freehandSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Freehand);  });
	connect(clearSelectionAction, &QAction::triggered, [this]() { clearSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_selectionType == SelectionType::Brush);
	
	freehandSelectionAction->setEnabled(false);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(clearSelectionAction);

	return selectionMenu;
}

void ImageViewerWidget::setupTextures()
{
	qDebug() << "Setup textures" << _imageSize;

	resetTextureData("image");
	resetTextureData("overlay");
	resetTextureData("selection");

	setupTexture(texture("image"));
	setupTexture(texture("overlay"), QOpenGLTexture::Filter::Nearest);
	setupTexture(texture("selection"), QOpenGLTexture::Filter::Nearest);
}

void ImageViewerWidget::setupTexture(QOpenGLTexture& openGltexture, const QOpenGLTexture::Filter& filter)
{
	openGltexture.destroy();
	openGltexture.create();
	openGltexture.setSize(_imageSize.width(), _imageSize.height(), 1);
	openGltexture.setFormat(QOpenGLTexture::RGBA8_UNorm);
	openGltexture.allocateStorage();
	openGltexture.setMinMagFilters(filter, filter);
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

	data.resize(_imageSize.width() * _imageSize.height() * 4);

	std::fill(data.begin(), data.end(), 0);
}

QOpenGLTexture& ImageViewerWidget::texture(const QString& name)
{
	return _textureMap.at(name);
}

TextureData& ImageViewerWidget::textureData(const QString& textureName)
{
	return _textureDataMap.at(textureName);
}
