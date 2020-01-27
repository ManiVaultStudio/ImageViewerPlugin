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
#include "SelectionOutline.h"

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7
// Line width and anti-aliasing inspired by // https://vitaliburkov.wordpress.com/2016/09/17/simple-and-fast-high-quality-antialiased-lines-with-opengl/

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_renderer(QSharedPointer<Renderer>::create(this)),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_renderer.get(), &Renderer::dirty, this, &ImageViewerWidget::onRendererDirty);
	connect(_renderer->selectionBufferQuad(), &SelectionBufferQuad::selectionEnded, this, &ImageViewerWidget::publishSelection);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, [&](std::shared_ptr<QImage> image, const QRect& bounds) {
		makeCurrent();
		_renderer->setSelectionImage(image, bounds);
		doneCurrent();
	}, Qt::AutoConnection);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, [&](std::shared_ptr<QImage> image) {
		makeCurrent();
		_renderer->setColorImage(image);
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

	_renderer->zoomExtents();

	_renderer->resize(QSize(w, h));
}

void ImageViewerWidget::paintGL() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

void ImageViewerWidget::onRendererDirty()
{
	//qDebug() << "Renderer dirty";

	update();
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
				_renderer->setSelectionType(SelectionType::Rectangle);
				break;

			case Qt::Key::Key_B:
				_renderer->setSelectionType(SelectionType::Brush);
				break;

			case Qt::Key::Key_L:
				_renderer->setSelectionType(SelectionType::Lasso);
				break;

			case Qt::Key::Key_P:
				_renderer->setSelectionType(SelectionType::Polygon);
				break;

			case Qt::Key::Key_Shift:
				_renderer->setSelectionModifier(SelectionModifier::Add);
				break;

			case Qt::Key::Key_Control:
				_renderer->setSelectionModifier(SelectionModifier::Remove);
				break;

			case Qt::Key::Key_Space:
				//startNavigationMode();
				break;

			default:
				break;
		}
		/**/
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
				_renderer->setSelectionModifier(SelectionModifier::Replace);
				break;
			}

			case Qt::Key::Key_Space:
				//endNavigationMode();
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

	makeCurrent();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_imageViewerPlugin->allowsPixelSelection()) {
				_renderer->setInteractionMode(InteractionMode::Selection);
				_renderer->selectionBufferQuad()->activate();
				_renderer->selectionOutline()->activate();
				_renderer->selectionOutline()->reset();
				_renderer->selectionBufferQuad()->reset();
			}

			break;
		}

		case Qt::RightButton:
		{
			_renderer->setInteractionMode(InteractionMode::WindowLevel);
			_renderer->imageQuad()->activate();
			break;
		}

		default:
			break;
	}

	_renderer->mousePressEvent(mouseEvent);

	doneCurrent();

	QOpenGLWidget::mousePressEvent(mouseEvent);
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent) {

	if (!_renderer->isInitialized())
		return;

	makeCurrent();

	if (mouseEvent->button() == Qt::RightButton)
	{
		if (_renderer->interactionMode() == InteractionMode::WindowLevel && _renderer->imageQuad()->mousePositions().size() == 1)
			contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_imageViewerPlugin->allowsPixelSelection()) {
				_renderer->selectionOutline()->reset();
				_renderer->selectionBufferQuad()->reset();
			}

			break;
		}

		case Qt::RightButton:
		{
			_renderer->setInteractionMode(InteractionMode::Selection);
			_renderer->imageQuad()->deactivate();
			break;
		}
		
		default:
			break;
		
	}

	_renderer->mouseReleaseEvent(mouseEvent);

	doneCurrent();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	if (!_renderer->isInitialized())
		return;

	makeCurrent();

	_renderer->mouseMoveEvent(mouseEvent);

	doneCurrent();
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!_renderer->isInitialized())
		return;

	makeCurrent();

	_renderer->mouseWheelEvent(wheelEvent);

	doneCurrent();
}

void ImageViewerWidget::publishSelection()
{	
	qDebug() << "Publish selection";
	
	const auto image = _renderer->selectionBufferQuad()->selectionBufferImage();

	auto pixelCoordinates = std::vector<std::pair<std::uint32_t, std::uint32_t>>();

	pixelCoordinates.reserve(image->width() * image->height());

	for (std::int32_t y = 0; y < image->height(); y++) {
		for (std::int32_t x = 0; x < image->width(); x++) {
			if (image->pixelColor(x, y).red() > 0) {
				pixelCoordinates.push_back(std::make_pair(x, y));
			}
		}
	}

	_imageViewerPlugin->selectPixels(pixelCoordinates, _renderer->selectionModifier());

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

	connect(zoomToExtentsAction, &QAction::triggered, this->_renderer.get(), &Renderer::zoomExtents);
	connect(zoomToSelectionAction, &QAction::triggered, this->_renderer.get(), &Renderer::zoomToSelection);
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

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { _renderer->setSelectionType(SelectionType::Rectangle); });
	connect(brushSelectionAction, &QAction::triggered, [this]() { _renderer->setSelectionType(SelectionType::Brush); });
	connect(lassoSelectionAction, &QAction::triggered, [this]() { _renderer->setSelectionType(SelectionType::Lasso); });
	connect(polygonSelectionAction, &QAction::triggered, [this]() { _renderer->setSelectionType(SelectionType::Polygon); });

	connect(selectAllAction, &QAction::triggered, [this]() { selectAll(); });
	connect(selectNoneAction, &QAction::triggered, [this]() { selectNone(); });
	connect(invertSelectionAction, &QAction::triggered, [this]() { invertSelection(); });

	rectangleSelectionAction->setCheckable(true);
	brushSelectionAction->setCheckable(true);
	lassoSelectionAction->setCheckable(true);
	polygonSelectionAction->setCheckable(true);

	const auto selectionType = _renderer->selectionType();

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