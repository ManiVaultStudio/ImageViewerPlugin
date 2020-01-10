#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include <vector>
#include <set>
#include <algorithm>

#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QList>
#include <QtMath>
#include <QPainter>
#include <QGuiApplication>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLDebugLogger>

#include "Shaders.h"

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_imageQuadRenderer(),
	_selectionRenderer(),
	_selectionBoundsRenderer(),
	_selectRenderer(),
	/*
	_overlayShaderProgram(),
	_selectionBoundsShaderProgram(),
	_pixelSelectionFBO(),
	*/
	_interactionMode(InteractionMode::Selection),
	_initialMousePosition(),
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selecting(false),
	_selectionType(SelectionType::Rectangle),
	_selectionModifier(SelectionModifier::Replace),
	_selectionOutlineColor(1.0f, 0.6f, 0.f, 1.0f),
	_ignorePaintGL(false),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewerWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionOpacityChanged, this, &ImageViewerWidget::onSelectionOpacityChanged);

	QSurfaceFormat surfaceFormat;

	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	surfaceFormat.setSamples(4);

#ifdef __APPLE__
	surfaceFormat.setVersion(3, 3);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#else
	surfaceFormat.setVersion(3, 3);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#endif

#ifdef _DEBUG
	surfaceFormat.setOption(QSurfaceFormat::DebugContext);
#endif

	setFormat(surfaceFormat);

	_imageQuadRenderer			= std::make_unique<ImageQuadRenderer>(0);
	_selectionRenderer			= std::make_unique<SelectionRenderer>(1);
	_selectionBoundsRenderer	= std::make_unique<SelectionBoundsRenderer>(2);
	_selectRenderer				= std::make_unique<SelectRenderer>(2);
}

ImageViewerWidget::~ImageViewerWidget()
{
	makeCurrent();

	_imageQuadRenderer->destroy();
	_selectionRenderer->destroy();
	_selectionBoundsRenderer->destroy();
	_selectRenderer->destroy();

	doneCurrent();
}

void ImageViewerWidget::startMouseInteraction()
{
	qDebug() << "Start mouse interaction";

	_initialMousePosition = _mousePosition;

	_mousePositions.clear();
}

void ImageViewerWidget::endMouseInteraction()
{
	qDebug() << "End mouse interaction";

	_mousePositions.clear();
}

void ImageViewerWidget::startNavigationMode()
{
	qDebug() << "Start navigation";

	setInteractionMode(InteractionMode::Navigation);

	startMouseInteraction();
}

void ImageViewerWidget::endNavigationMode()
{
	qDebug() << "End navigation";

	endMouseInteraction();

	QWidget::setCursor(Qt::OpenHandCursor);

	setInteractionMode(InteractionMode::None);
}

void ImageViewerWidget::startSelectionMode(const SelectionType& selectionType)
{
	qDebug() << "Start selection mode";

	setInteractionMode(InteractionMode::Selection);
	setSelectionType(selectionType);

	startMouseInteraction();
}

void ImageViewerWidget::endSelectionMode()
{
	qDebug() << "End selection mode";

	endMouseInteraction();

	setInteractionMode(InteractionMode::None);
}

void ImageViewerWidget::startSelection()
{
	qDebug() << "Start selection";

	startMouseInteraction();

	_selecting = true;
}

void ImageViewerWidget::endSelection()
{
	qDebug() << "End selection";

	endMouseInteraction();

	_selecting = false;

	publishSelection();
}

void ImageViewerWidget::startWindowLevelMode()
{
	qDebug() << "Start window/level interaction";

	setInteractionMode(InteractionMode::WindowLevel);

	startMouseInteraction();
}

void ImageViewerWidget::endWindowLevelMode()
{
	qDebug() << "End window/level interaction";

	endMouseInteraction();

	setInteractionMode(InteractionMode::Selection);
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glDepthMask(false);

	_imageQuadRenderer->init();
	_selectionRenderer->init();
	_selectionBoundsRenderer->init();
	_selectRenderer->init();

	_imageViewerPlugin->computeDisplayImage();
	_imageViewerPlugin->computeSelectionImage();
	
#ifdef _DEBUG
	_openglDebugLogger->initialize();
#endif
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";

	zoomExtents();

	_imageQuadRenderer->resize(QSize(w, h));
	_selectionRenderer->resize(QSize(w, h));
	_selectionBoundsRenderer->resize(QSize(w, h));
	_selectRenderer->resize(QSize(w, h));
}

void ImageViewerWidget::paintGL() {

	if (_ignorePaintGL)
		return;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto modelViewProjection = projection() * modelView();

	/*
	if (_overlayShaderProgram->isLinked()) {
	if (_interactionMode == InteractionMode::Selection && _selectionOutlineShaderProgram->bind()) {
		QMatrix4x4 transform;

		transform.ortho(rect());

		_selectionOutlineShaderProgram->setUniformValue("transform", transform);
		_selectionOutlineShaderProgram->setUniformValue("color", _selectionOutlineColor);

		drawSelectionOutline();

		_selectionOutlineShaderProgram->release();
	}
	*/
	
	_imageQuadRenderer->setModelViewProjection(modelViewProjection);
	_imageQuadRenderer->render();
	
	_selectionRenderer->setModelViewProjection(modelViewProjection);
	_selectionRenderer->render();
	
	_selectionBoundsRenderer->setModelViewProjection(modelViewProjection);
	_selectionBoundsRenderer->render();
	
	_selectRenderer->setModelViewProjection(modelViewProjection);
	_selectRenderer->render();
	
#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		qDebug() << message;
#endif
}

void ImageViewerWidget::onDisplayImageChanged(std::shared_ptr<QImage> displayImage)
{
	if (!isValid())
		return;
	
	makeCurrent();

	auto imageSizeChanged = _imageQuadRenderer->size() != displayImage->size();

	_imageQuadRenderer->setImage(displayImage);

	_selectRenderer->setImageSize(displayImage->size());

	if (imageSizeChanged) {
		zoomExtents();

		const auto brushRadius = 0.05f * static_cast<float>(std::min(_imageQuadRenderer->size().width(), _imageQuadRenderer->size().height()));

		_selectRenderer->setBrushRadius(brushRadius);
		_selectRenderer->setBrushRadiusDelta(0.2f * brushRadius);
	}

	doneCurrent();

	update();
}

void ImageViewerWidget::onSelectionImageChanged(std::shared_ptr<QImage> selectionImage, const QRect& selectionBounds)
{
	if (!isValid())
		return;

	makeCurrent();

	_selectionRenderer->setImage(selectionImage);

	const auto worldSelectionBounds = QRect(selectionBounds.left(), selectionImage->height() - selectionBounds.bottom() - 1, selectionBounds.width() + 1, selectionBounds.height() + 1);

	_selectionBoundsRenderer->setSelectionBounds(worldSelectionBounds);

	doneCurrent();

	update();
}

void ImageViewerWidget::onSelectionOpacityChanged(const float& selectionOpacity)
{
	_selectionRenderer->setOpacity(selectionOpacity);

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
//	endSelection();
}

void ImageViewerWidget::onCurrentImageIdChanged(const std::int32_t& currentImageId)
{
//	endSelection();
}

void ImageViewerWidget::keyPressEvent(QKeyEvent* keyEvent)
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
				startSelectionMode(SelectionType::Rectangle);
				break;

			case Qt::Key::Key_B:
				startSelectionMode(SelectionType::Brush);
				break;

			case Qt::Key::Key_L:
				startSelectionMode(SelectionType::Lasso);
				break;

			case Qt::Key::Key_P:
				startSelectionMode(SelectionType::Polygon);
				break;

			case Qt::Key::Key_Shift:
				setSelectionModifier(SelectionModifier::Add);
				break;

			case Qt::Key::Key_Control:
				setSelectionModifier(SelectionModifier::Remove);
				break;

			case Qt::Key::Key_Space:
				startNavigationMode();
				break;

			default:
				break;
		}
	}

	QOpenGLWidget::keyPressEvent(keyEvent);
}

void ImageViewerWidget::keyReleaseEvent(QKeyEvent* keyEvent)
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
				setSelectionModifier(SelectionModifier::Replace);
				break;
			}

			case Qt::Key::Key_Space:
				endNavigationMode();
				break;

			default:
				break;
		}
	}

	QOpenGLWidget::keyReleaseEvent(keyEvent);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (!initialized())
		return;

	qDebug() << "Mouse press event";

	makeCurrent();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			_mousePosition = mouseEvent->pos();

			if (_imageViewerPlugin->selectable()) {
				if (_mousePositions.empty())
					startSelection();

				if (_selectionType != SelectionType::Polygon) {
					_selectRenderer->resetPixelSelection();
				}

				_mousePositions.push_back(_mousePosition);

				auto worldMousePositions = std::vector<QVector3D>();

				for (const auto& mousePosition : _mousePositions)
				{
					worldMousePositions.push_back(screenToWorld(mousePosition));
				}
				_selectRenderer->updatePixelSelection(_selectionType, worldMousePositions);
			}

			break;
		}

		case Qt::RightButton:
		{
			if (_selectionType == SelectionType::Polygon && !_mousePositions.empty()) {
				endSelection();
			}
			else {
				startWindowLevelMode();
			}
			
			break;
		}

		default:
			break;
	}

	doneCurrent();
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	if (!initialized())
		return;

	if (!_mousePositions.empty() && mouseEvent->pos() == _mousePositions.back())
		return;

	//qDebug() << "Mouse move event";

	makeCurrent();

	switch (mouseEvent->buttons())
	{
		case Qt::LeftButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
				{
					pan(QPointF(mouseEvent->pos().x() - _mousePosition.x(), mouseEvent->pos().y() - _mousePosition.y()));
					break;
				}

				case InteractionMode::Selection:
				{
					if (_imageViewerPlugin->selectable() && _selecting) {
						if (_selectionType != SelectionType::Polygon) {
							_mousePositions.push_back(mouseEvent->pos());
						}

						auto worldMousePositions = std::vector<QVector3D>();

						for (const auto& mousePosition : _mousePositions)
						{
							worldMousePositions.push_back(screenToWorld(mousePosition));
						}
						_selectRenderer->updatePixelSelection(_selectionType, worldMousePositions);
					}
					
					break;
				}

				default:
					break;
			}

			break;
		}

		case Qt::RightButton:
		{
			if (_interactionMode == InteractionMode::WindowLevel) {
				const auto worldPos = screenToWorld(_mousePosition);
				const auto deltaWindow = (mouseEvent->pos().x() - _mousePosition.x()) / 150.f;
				const auto deltaLevel = -(mouseEvent->pos().y() - _mousePosition.y()) / 150.f;
				const auto window = std::clamp(_imageQuadRenderer->window() + deltaWindow, 0.0f, 1.0f);
				const auto level = std::clamp(_imageQuadRenderer->level() + deltaLevel, 0.0f, 1.0f);

				_imageQuadRenderer->setWindowLevel(window, level);

				_mousePositions.push_back(_mousePosition);
			}

			break;
		}

		default:
			break;
	}

	doneCurrent();

	update();

	_mousePosition = mouseEvent->pos();
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent) {

	if (!initialized())
		return;

	qDebug() << "Mouse release event";

	if (mouseEvent->button() == Qt::RightButton && _mousePositions.size() == 0)
	{
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_interactionMode == InteractionMode::Selection)
			{
				if (_imageViewerPlugin->selectable() && _selecting) {
					if (_selectionType != SelectionType::Polygon) {
						endSelection();
					}
				}

				break;
			}
		}

		case Qt::RightButton:
		{
			switch (_interactionMode)
			{
				case InteractionMode::WindowLevel:
				{
					endWindowLevelMode();
					break;
				}

				default:
					break;
			}
		
			break;
		}
		
		default:
			break;
		
	}
	
	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!initialized())
		return;

	qDebug() << "Mouse wheel event" << interactionModeTypeName(_interactionMode);

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			const auto world_x = (wheelEvent->posF().x() - _pan.x()) / _zoom;
			const auto world_y = (wheelEvent->posF().y() - _pan.y()) / _zoom;

			auto zoomCenter = wheelEvent->posF();

			//zoomCenter.setY(height() - wheelEvent->posF().y());

			if (wheelEvent->delta() > 0) {
				zoomAt(zoomCenter, 1.f - _zoomSensitivity);
			}
			else {
				zoomAt(zoomCenter, 1.f + _zoomSensitivity);
			}

			update();
			break;
		}

		case InteractionMode::Selection:
		{
			if (_selectionType == SelectionType::Brush) {
				if (wheelEvent->delta() > 0) {
					_selectRenderer->brushSizeIncrease();
				}
				else {
					_selectRenderer->brushSizeDecrease();
				}

				update();
			}

			break;
		}

		case InteractionMode::WindowLevel:
			break;

		default:
			break;
	}
}

void ImageViewerWidget::pan(const QPointF& delta)
{
	qDebug() << "Pan" << delta;

	_pan.setX(_pan.x() + (delta.x() / _zoom));
	_pan.setY(_pan.y() + (delta.y() / _zoom));
}

void ImageViewerWidget::zoom(const float& factor)
{
	_zoom *= factor;

	qDebug() << "Zoom" << _zoom;

	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);
}

void ImageViewerWidget::zoomAt(const QPointF& screenPosition, const float& factor) {

	qDebug() << "Zoom at" << screenPosition << factor;

	//pan(QPointF(-screenPosition.x(), -screenPosition.y()));
	zoom(factor);
	//pan(QPointF(screenPosition.x(), screenPosition.y()));
}

void ImageViewerWidget::zoomExtents()
{
	if (_imageViewerPlugin->currentDatasetName().isEmpty())
		return;
	
	qDebug() << "Zoom extents" << _zoom;

	zoomToRectangle(QRectF(QPointF(), QSizeF(_imageQuadRenderer->size().width(), _imageQuadRenderer->size().height())));
}

void ImageViewerWidget::zoomToRectangle(const QRectF& rectangle)
{
	qDebug() << "Zoom to rectangle" << rectangle;

	resetView();
	
	const auto center	= rectangle.center();
	const auto factorX	= (width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (height() - 2 * _margin) / static_cast<float>(rectangle.height());

	zoom(factorX < factorY ? factorX : factorY);
	pan(_zoom * -QPointF(center.x(), _imageQuadRenderer->size().height() - center.y()));

	update();
}

void ImageViewerWidget::zoomToSelection()
{
	auto* currentImageDataSet = _imageViewerPlugin->currentImages();

	if (currentImageDataSet == nullptr)
		return;

	qDebug() << "Zoom to selection";

	zoomToRectangle(QRectF(currentImageDataSet->selectionBounds(true)));
}

void ImageViewerWidget::resetView()
{
	qDebug() << "Reset view";

	_pan.setX(0);
	_pan.setY(0);

	_zoom = 1.f;

	update();
}

bool ImageViewerWidget::initialized()
{
	return true;
	//return _displayImage.get() != nullptr;
}

QVector3D ImageViewerWidget::screenToWorld(const QPoint& screenPoint) const
{
	return QVector3D(screenPoint.x(), height() - screenPoint.y(), 0).unproject(modelView(), projection(), QRect(0, 0, width(), height()));
}

void ImageViewerWidget::publishSelection()
{
	/*
	qDebug() << "Publish selection";
	
	makeCurrent();

	const auto image = _pixelSelectionFBO->toImage();

	doneCurrent();

	resetPixelSelection();

	auto pixelCoordinates = std::vector<std::pair<std::uint32_t, std::uint32_t>>();

	pixelCoordinates.reserve(image.width() * image.height());

	for (std::int32_t y = 0; y < image.height(); y++) {
		for (std::int32_t x = 0; x < image.width(); x++) {
			if (image.pixelColor(x, y).red() > 0) {
				pixelCoordinates.push_back(std::make_pair(x, y));
			}
		}
	}

	_imageViewerPlugin->selectPixels(pixelCoordinates, _selectionModifier);

	update();
	*/
}

void ImageViewerWidget::selectAll()
{
	qDebug() << "Select all";

	_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::All);

	update();
}

void ImageViewerWidget::selectNone()
{
	qDebug() << "Select none";

	_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::None);

	update();
}

void ImageViewerWidget::invertSelection()
{
	qDebug() << "Invert selection";

	_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::Invert);

	update();
}

QMenu* ImageViewerWidget::contextMenu()
{
	auto* contextMenu = new QMenu();

	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		contextMenu->addMenu(viewMenu());
		contextMenu->addSeparator();
		contextMenu->addMenu(selectionMenu());

		qDebug() << _imageViewerPlugin->noSelectedPixels();

		if (_imageViewerPlugin->noSelectedPixels() > 0) {
			contextMenu->addSeparator();

			auto* createSubsetFromSelectionAction = new QAction("Create subset from selection");

			connect(createSubsetFromSelectionAction, &QAction::triggered, _imageViewerPlugin, &ImageViewerPlugin::createSubsetFromSelection);

			contextMenu->addAction(createSubsetFromSelectionAction);
		}
	}

	return contextMenu;
}

QMenu* ImageViewerWidget::viewMenu()
{
	auto* viewMenu = new QMenu("View");

	auto* zoomToExtentsAction = new QAction("Zoom extents");
	auto* zoomToSelectionAction = new QAction("Zoom to selection");
	auto* resetWindowLevelAction = new QAction("Reset window/level");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");
	zoomToSelectionAction->setToolTip("Zoom to selection boundaries");
	resetWindowLevelAction->setToolTip("Reset window/level to default values");

	zoomToSelectionAction->setEnabled(_imageViewerPlugin->noSelectedPixels() > 0);

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);
	connect(zoomToSelectionAction, &QAction::triggered, this, &ImageViewerWidget::zoomToSelection);
	connect(resetWindowLevelAction, &QAction::triggered, [&]() { _imageQuadRenderer->resetWindowLevel();  });

	viewMenu->addAction(zoomToExtentsAction);
	viewMenu->addAction(zoomToSelectionAction);
	viewMenu->addSeparator();
	viewMenu->addAction(resetWindowLevelAction);

	return viewMenu;
}

QMenu* ImageViewerWidget::selectionMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction	= new QAction("Rectangle");
	auto* brushSelectionAction		= new QAction("Brush");
	auto* lassoSelectionAction		= new QAction("Lasso", this);
	auto* polygonSelectionAction	= new QAction("Polygon", this);
	auto* selectNoneAction			= new QAction("Select none");
	auto* selectAllAction			= new QAction("Select all");
	auto* invertSelectionAction		= new QAction("Invert");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle);  });
	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush);  });
	connect(lassoSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Lasso);  });
	connect(polygonSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Polygon);  });

	connect(selectAllAction, &QAction::triggered, [this]() { selectAll(); });
	connect(selectNoneAction, &QAction::triggered, [this]() { selectNone(); });
	connect(invertSelectionAction, &QAction::triggered, [this]() { invertSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);
	lassoSelectionAction->setCheckable(true);
	polygonSelectionAction->setCheckable(true);

	rectangleSelectionAction->setChecked(_selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(_selectionType == SelectionType::Brush);
	lassoSelectionAction->setChecked(_selectionType == SelectionType::Lasso);
	polygonSelectionAction->setChecked(_selectionType == SelectionType::Polygon);

	selectionMenu->addAction(rectangleSelectionAction);
	selectionMenu->addAction(brushSelectionAction);
	selectionMenu->addAction(lassoSelectionAction);
	selectionMenu->addAction(polygonSelectionAction);
	selectionMenu->addSeparator();
	selectionMenu->addAction(selectAllAction);
	selectionMenu->addAction(selectNoneAction);
	selectionMenu->addAction(invertSelectionAction);

	return selectionMenu;
}

QMatrix4x4 ImageViewerWidget::modelView() const
{
	QMatrix4x4 model, view;

	model.scale(_zoom, _zoom, 1.0f);
	model.translate(_pan.x(), _pan.y());
	view.lookAt(QVector3D(0, 0, -1), QVector3D(0, 0, 0), QVector3D(0, -1, 0));

	return view * model;
}

QMatrix4x4 ImageViewerWidget::projection() const
{
	const auto halfSize = size() / 2;

	QMatrix4x4 projection;

	projection.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -10.0f, +10.0f);

	return projection;
}

InteractionMode ImageViewerWidget::interactionMode() const
{
	return _interactionMode;
}

void ImageViewerWidget::setInteractionMode(const InteractionMode& interactionMode)
{
	if (interactionMode == _interactionMode)
		return;

	qDebug() << "Set interaction mode to" << interactionModeTypeName(interactionMode);

	switch (interactionMode)
	{
		case InteractionMode::Navigation:
			QWidget::setCursor(Qt::OpenHandCursor);
			break;

		case InteractionMode::Selection:
		case InteractionMode::None:
			QWidget::setCursor(Qt::ArrowCursor);
			break;

		default:
			break;
	}

	_interactionMode = interactionMode;
}

SelectionType ImageViewerWidget::selectionType() const
{
	return _selectionType;
}

void ImageViewerWidget::setSelectionType(const SelectionType& selectionType)
{
	if (selectionType == _selectionType)
		return;

	qDebug() << "Set selection type to" << selectionTypeName(selectionType);

	_selectionType = selectionType;

	update();
}

SelectionModifier ImageViewerWidget::selectionModifier() const
{
	return _selectionModifier;
}

void ImageViewerWidget::setSelectionModifier(const SelectionModifier& selectionModifier)
{
	if (selectionModifier == _selectionModifier)
		return;

	qDebug() << "Set selection modifier to" << selectionModifierName(selectionModifier);

	_selectionModifier = selectionModifier;
}

void ImageViewerWidget::drawSelectionOutlineRectangle(const QPoint& start, const QPoint& end)
{
	/*
	const GLfloat vertexCoordinates[] = {
	  start.x(), start.y(), 0.0f,
	  end.x(), start.y(), 0.0f,
	  end.x(), end.y(), 0.0f,
	  start.x(), end.y(), 0.0f
	};

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates, 3);

	glDrawArrays(GL_LINE_LOOP, 0, 4);
	*/
}

void ImageViewerWidget::drawSelectionOutlineBrush()
{
	/*
	const auto brushCenter	= QWidget::mapFromGlobal(QCursor::pos());
	const auto noSegments	= 64u;

	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(noSegments * 3);

	const auto brushRadius = _brushRadius * _zoom;

	for (std::uint32_t s = 0; s < noSegments; s++) {
		const auto theta	= 2.0f * M_PI * float(s) / float(noSegments);
		const auto x		= brushRadius * cosf(theta);
		const auto y		= brushRadius * sinf(theta);

		vertexCoordinates[s * 3 + 0] = brushCenter.x() + x;
		vertexCoordinates[s * 3 + 1] = brushCenter.y() + y;
		vertexCoordinates[s * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glDrawArrays(GL_LINE_LOOP, 0, noSegments);
	*/
}

void ImageViewerWidget::drawSelectionOutlineLasso()
{
	/*
	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(_mousePositions.size() * 3);

	for (std::size_t p = 0; p < _mousePositions.size(); p++) {
		const auto mousePosition = _mousePositions[p];

		vertexCoordinates[p * 3 + 0] = mousePosition.x();
		vertexCoordinates[p * 3 + 1] = mousePosition.y();
		vertexCoordinates[p * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);
	
	glDrawArrays(GL_LINE_LOOP, 0, static_cast<std::int32_t>(_mousePositions.size()));
	*/
}

void ImageViewerWidget::drawSelectionOutlinePolygon()
{
	/*
	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(_mousePositions.size() * 3);

	for (std::size_t p = 0; p < _mousePositions.size(); p++) {
		const auto mousePosition = _mousePositions[p];

		vertexCoordinates[p * 3 + 0] = mousePosition.x();
		vertexCoordinates[p * 3 + 1] = mousePosition.y();
		vertexCoordinates[p * 3 + 2] = 0.f;
	}

	const auto vertexLocation = _selectionOutlineShaderProgram->attributeLocation("vertex");

	_selectionOutlineShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glPointSize(4.0f);

	glDrawArrays(GL_LINE_LOOP, 0, static_cast<std::int32_t>(_mousePositions.size()));
	glDrawArrays(GL_POINTS, 0, static_cast<std::int32_t>(_mousePositions.size()));
	*/
}

void ImageViewerWidget::drawSelectionOutline()
{
	//qDebug() << "Draw selection outline";

	//glEnable(GL_LINE_STIPPLE);
	//glLineStipple(1, 0x0101);
	//glLineWidth(2.5f);

	switch (_selectionType)
	{
		case SelectionType::Rectangle:
		{
			if (_selecting) {
				const auto currentMouseWorldPos = QWidget::mapFromGlobal(QCursor::pos());
				drawSelectionOutlineRectangle(_initialMousePosition, currentMouseWorldPos);
			}

			break;
		}

		case SelectionType::Brush:
		{
			drawSelectionOutlineBrush();
			break;
		}

		case SelectionType::Lasso:
		{
			drawSelectionOutlineLasso();
			break;
		}

		case SelectionType::Polygon:
		{
			drawSelectionOutlinePolygon();
			break;
		}

		default:
			break;
	}

	//glDisable(GL_LINE_STIPPLE);
}