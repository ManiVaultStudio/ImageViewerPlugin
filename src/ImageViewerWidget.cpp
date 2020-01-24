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
#include <QGuiApplication>
#include <QOpenGLDebugLogger>

#include "Renderer.h"
#include "ImageQuad.h"
#include "SelectionBufferQuad.h"

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7
// Line width and anti-aliasing inspired by // https://vitaliburkov.wordpress.com/2016/09/17/simple-and-fast-high-quality-antialiased-lines-with-opengl/

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_renderer(QSharedPointer<Renderer>::create(3, this)),
	_interactionMode(InteractionMode::Selection),
	_mousePosition(),
	_zoom(1.f),
	_zoomSensitivity(0.05f),
	_margin(25),
	_selecting(false),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, [&](std::shared_ptr<QImage> image, const QRect& bounds) {
		makeCurrent();
		{
			_renderer->setSelectionImage(image, bounds);
			update();
		}
		doneCurrent();
	}, Qt::AutoConnection);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, [&](std::shared_ptr<QImage> image) {
		makeCurrent();
		{
			_renderer->setColorImage(image);
			zoomExtents();
			update();
		}
		doneCurrent();
	}, Qt::AutoConnection);

	QSurfaceFormat surfaceFormat;

	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	surfaceFormat.setSamples(4);

#ifdef __APPLE__
	// Ask for an OpenGL 3.3 Core Context as the default
	surfaceFormat.setVersion(3, 3);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	//QSurfaceFormat::setDefaultFormat(defaultFormat);
#else
	// Ask for an OpenGL 4.3 Core Context as the default
	surfaceFormat.setVersion(4, 3);
	surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	surfaceFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
#endif

#ifdef _DEBUG
	surfaceFormat.setOption(QSurfaceFormat::DebugContext);
#endif
	
	surfaceFormat.setSamples(16);

	setFormat(surfaceFormat);
}

ImageViewerWidget::~ImageViewerWidget()
{
	makeCurrent();

	_renderer->destroy();

	doneCurrent();
}

void ImageViewerWidget::startMouseInteraction()
{
	qDebug() << "Start mouse interaction";

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

	_renderer->selectionBufferQuad()->setSelectionType(selectionType);

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

	makeCurrent();

	_renderer->selectionBufferQuad()->reset();

	doneCurrent();
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	glEnable(GL_MULTISAMPLE);

	_renderer->init();

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

	_renderer->resize(QSize(w, h));
}

void ImageViewerWidget::paintGL() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto modelViewProjection = projection() * modelView();

	_renderer->setModelViewProjection(modelViewProjection);
	_renderer->render();

#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		qDebug() << message;
#endif
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
				_renderer->selectionBufferQuad()->setSelectionModifier(SelectionModifier::Add);
				break;

			case Qt::Key::Key_Control:
				_renderer->selectionBufferQuad()->setSelectionModifier(SelectionModifier::Remove);
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
				_renderer->selectionBufferQuad()->setSelectionModifier(SelectionModifier::Replace);
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
	if (!_renderer->isInitialized())
		return;

	qDebug() << "Mouse press event";

	makeCurrent();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			_mousePosition = mouseEvent->pos();

			if (_imageViewerPlugin->allowsPixelSelection()) {
				if (_mousePositions.empty())
					startSelection();

				if (_renderer->selectionBufferQuad()->selectionType() != SelectionType::Polygon) {
					_renderer->selectionBufferQuad()->reset();
				}

				_mousePositions.push_back(_mousePosition);

				_renderer->selectionBufferQuad()->setMousePositions(mousePositionsWorld());
			}

			break;
		}

		case Qt::RightButton:
		{
			if (_renderer->selectionBufferQuad()->selectionType() == SelectionType::Polygon && !_mousePositions.empty()) {
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

	if (!_renderer->isInitialized())
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
					/*
					auto worldMousePositions = std::vector<QVector3D>();

					for (const auto& mousePosition : _mousePositions)
					{
						worldMousePositions.push_back(screenToWorld(mousePosition));
					}
					*/

					if (_imageViewerPlugin->allowsPixelSelection() && _selecting) {
						_mousePositions.push_back(mouseEvent->pos());
						/*
						if (_renderer->selectionBufferQuad()->selectionType() != SelectionType::Polygon) {
							const auto lastMousePosition = _mousePositions.back();
							const auto moved = mouseEvent->pos() - lastMousePosition;

							if (moved.manhattanLength() > 10)
								_mousePositions.push_back(mouseEvent->pos());
						}
						*/

						_renderer->selectionBufferQuad()->setMousePositions(mousePositionsWorld());
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
				const auto worldPos		= screenToWorld(_mousePosition);
				const auto deltaWindow	= (mouseEvent->pos().x() - _mousePosition.x()) / 150.f;
				const auto deltaLevel	= -(mouseEvent->pos().y() - _mousePosition.y()) / 150.f;
				const auto window		= std::clamp(_renderer->imageQuad()->windowNormalized() + deltaWindow, 0.0f, 1.0f);
				const auto level		= std::clamp(_renderer->imageQuad()->levelNormalized() + deltaLevel, 0.0f, 1.0f);

				_renderer->imageQuad()->setWindowLevel(window, level);

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

	if (!_renderer->isInitialized())
		return;

	qDebug() << "Mouse release event";

	if (mouseEvent->button() == Qt::RightButton && _mousePositions.size() == 0 && _interactionMode != InteractionMode::Selection)
	{
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	makeCurrent();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_interactionMode == InteractionMode::Selection)
			{
				if (_imageViewerPlugin->allowsPixelSelection() && _selecting) {
					if (_renderer->selectionBufferQuad()->selectionType() != SelectionType::Polygon) {
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
	
	doneCurrent();

	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!_renderer->isInitialized())
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
			if (_renderer->selectionBufferQuad()->selectionType() == SelectionType::Brush) {
				if (wheelEvent->delta() > 0) {
					_renderer->selectionBufferQuad()->brushSizeIncrease();
				}
				else {
					_renderer->selectionBufferQuad()->brushSizeDecrease();
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

float ImageViewerWidget::zoom() const
{
	return _zoom;
}

void ImageViewerWidget::zoomBy(const float& factor)
{
	_zoom *= factor;

	qDebug() << "Zoom" << _zoom;

	_pan.setX(_pan.x() * factor);
	_pan.setY(_pan.y() * factor);
}

void ImageViewerWidget::zoomAt(const QPointF& screenPosition, const float& factor) {

	qDebug() << "Zoom at" << screenPosition << factor;

	//pan(QPointF(-screenPosition.x(), -screenPosition.y()));
	zoomBy(factor);
	//pan(QPointF(screenPosition.x(), screenPosition.y()));
}

void ImageViewerWidget::zoomExtents()
{
	if (_imageViewerPlugin->currentDatasetName().isEmpty())
		return;
	
	qDebug() << "Zoom extents" << _zoom;

	auto* imageQuad = _renderer->shape<ImageQuad>("ImageQuad");

	zoomToRectangle(QRectF(QPointF(), QSizeF(imageQuad->size().width(), imageQuad->size().height())));
}

void ImageViewerWidget::zoomToRectangle(const QRectF& rectangle)
{
	qDebug() << "Zoom to rectangle" << rectangle;

	resetView();
	
	const auto center	= rectangle.center();
	const auto factorX	= (width() - 2 * _margin) / static_cast<float>(rectangle.width());
	const auto factorY	= (height() - 2 * _margin) / static_cast<float>(rectangle.height());

	zoomBy(factorX < factorY ? factorX : factorY);

	auto* imageQuad = _renderer->shape<ImageQuad>("ImageQuad");

	pan(_zoom * -QPointF(center.x(), imageQuad->size().height() - center.y()));

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

QVector3D ImageViewerWidget::screenToWorld(const QPointF& screen) const
{
	return QVector3D(screen.x(), height() - screen.y(), 0).unproject(modelView(), projection(), QRect(0, 0, width(), height()));
}

void ImageViewerWidget::publishSelection()
{	
	qDebug() << "Publish selection";
	
	const auto image = _renderer->selectionBufferQuad()->selectionImage();

	auto pixelCoordinates = std::vector<std::pair<std::uint32_t, std::uint32_t>>();

	pixelCoordinates.reserve(image->width() * image->height());

	for (std::int32_t y = 0; y < image->height(); y++) {
		for (std::int32_t x = 0; x < image->width(); x++) {
			if (image->pixelColor(x, y).red() > 0) {
				pixelCoordinates.push_back(std::make_pair(x, y));
			}
		}
	}

	_imageViewerPlugin->selectPixels(pixelCoordinates, _renderer->selectionBufferQuad()->selectionModifier());

	_renderer->selectionBufferQuad()->reset();

	update();
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

QSharedPointer<Renderer> ImageViewerWidget::renderer()
{
	return _renderer;
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
	resetWindowLevelAction->setEnabled(_renderer->imageQuad()->windowNormalized() < 1.f && _renderer->imageQuad()->levelNormalized() != 0.5f);

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);
	connect(zoomToSelectionAction, &QAction::triggered, this, &ImageViewerWidget::zoomToSelection);
	connect(resetWindowLevelAction, &QAction::triggered, [&]() { _renderer->imageQuad()->resetWindowLevel();  });

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

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { _renderer->selectionBufferQuad()->setSelectionType(SelectionType::Rectangle); });
	connect(brushSelectionAction, &QAction::triggered, [this]() { _renderer->selectionBufferQuad()->setSelectionType(SelectionType::Brush); });
	connect(lassoSelectionAction, &QAction::triggered, [this]() { _renderer->selectionBufferQuad()->setSelectionType(SelectionType::Lasso); });
	connect(polygonSelectionAction, &QAction::triggered, [this]() { _renderer->selectionBufferQuad()->setSelectionType(SelectionType::Polygon); });

	connect(selectAllAction, &QAction::triggered, [this]() { selectAll(); });
	connect(selectNoneAction, &QAction::triggered, [this]() { selectNone(); });
	connect(invertSelectionAction, &QAction::triggered, [this]() { invertSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);
	lassoSelectionAction->setCheckable(true);
	polygonSelectionAction->setCheckable(true);

	const auto selectionType = _renderer->selectionBufferQuad()->selectionType();

	rectangleSelectionAction->setChecked(selectionType == SelectionType::Rectangle);
	brushSelectionAction->setChecked(selectionType == SelectionType::Brush);
	lassoSelectionAction->setChecked(selectionType == SelectionType::Lasso);
	polygonSelectionAction->setChecked(selectionType == SelectionType::Polygon);

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

	projection.ortho(-halfSize.width(), halfSize.width(), -halfSize.height(), halfSize.height(), -100.0f, +100.0f);

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

QPoint ImageViewerWidget::mousePosition() const
{
	return _mousePosition;
}

std::vector<QPoint> ImageViewerWidget::mousePositionsScreen() const
{
	return _mousePositions;
}

std::vector<QVector3D> ImageViewerWidget::mousePositionsWorld() const
{
	auto mousePositionsWorld = std::vector<QVector3D>();

	for (const auto& mousePosition : _mousePositions)
	{
		mousePositionsWorld.push_back(screenToWorld(mousePosition));
	}

	return mousePositionsWorld;
}

bool ImageViewerWidget::selecting() const
{
	return _selecting;
}