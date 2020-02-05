#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include <vector>
#include <algorithm>

#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QGuiApplication>
#include <QOpenGLDebugLogger>

#include "Renderer.h"

#include "SelectionPickerActor.h"

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_initialMousePosition(),
	_renderer(QSharedPointer<Renderer>::create(this)),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_renderer.get(), &Renderer::dirty, this, &ImageViewerWidget::onRendererDirty);

	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, [&](std::shared_ptr<QImage> image, const QRect& bounds) {
		_renderer->setSelectionImage(image, bounds);
	}, Qt::AutoConnection);

	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, [&](std::shared_ptr<QImage> image) {
		_renderer->setColorImage(image);

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
	_renderer->destroy();
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	makeCurrent();
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);

	glEnable(GL_MULTISAMPLE);

	_renderer->init();

	_imageViewerPlugin->computeDisplayImage();
	_imageViewerPlugin->computeSelectionImage();

	//connect(_renderer->selectionBufferQuad(), &SelectionBufferQuad::selectionEnded, this, &ImageViewerWidget::publishSelection);
	
	connect(_renderer.get(), &Renderer::selectAll, [&]() {
		_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::All);
		update();
	});

	connect(_renderer.get(), &Renderer::selectNone, [&]() {
		_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::None);
		update();
	});

	connect(_renderer.get(), &Renderer::selectInvert, [&]() {
		_imageViewerPlugin->selectPixels(std::vector<std::pair<std::uint32_t, std::uint32_t>>(), SelectionModifier::Invert);
		update();
	});

#ifdef _DEBUG
//	_openglDebugLogger->initialize();
#endif

	doneCurrent();
}

void ImageViewerWidget::resizeGL(int w, int h)
{
//	_renderer->resize(QSize(w, h));
}

void ImageViewerWidget::paintGL() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_renderer->render();

	/*
#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		qDebug() << message;
#endif
*/
}

void ImageViewerWidget::onRendererDirty()
{
	//qDebug() << "Renderer dirty";

	update();
}

void ImageViewerWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	_renderer->keyPressEvent(keyEvent);

	QOpenGLWidget::keyPressEvent(keyEvent);
}

void ImageViewerWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	_renderer->keyReleaseEvent(keyEvent);

	QOpenGLWidget::keyReleaseEvent(keyEvent);
}

void ImageViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	_initialMousePosition = mouseEvent->pos();

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_renderer->interactionMode() != InteractionMode::Navigation && _imageViewerPlugin->allowsPixelSelection()) {
				_renderer->setInteractionMode(InteractionMode::Selection);
			}

			break;
		}

		default:
			break;
	}

	_renderer->mousePressEvent(mouseEvent);

	QOpenGLWidget::mousePressEvent(mouseEvent);
}

void ImageViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (mouseEvent->button() == Qt::RightButton && _renderer->allowsContextMenu()) {
		_renderer->contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	_renderer->mouseReleaseEvent(mouseEvent);

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	_renderer->mouseMoveEvent(mouseEvent);

	QOpenGLWidget::mouseMoveEvent(mouseEvent);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	_renderer->mouseWheelEvent(wheelEvent);
}

void ImageViewerWidget::publishSelection()
{	
	qDebug() << "Publish selection";
	
	/*
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
	*/

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
		contextMenu->addMenu(_renderer->contextMenu());
		contextMenu->addSeparator();
		contextMenu->addMenu(_renderer->actorByName<SelectionPickerActor>("SelectionPickerActor")->contextMenu());

		if (_imageViewerPlugin->noSelectedPixels() > 0) {
			contextMenu->addSeparator();

			auto* createSubsetFromSelectionAction = new QAction("Create subset from selection");

			connect(createSubsetFromSelectionAction, &QAction::triggered, _imageViewerPlugin, &ImageViewerPlugin::createSubsetFromSelection);

			contextMenu->addAction(createSubsetFromSelectionAction);
		}
	}

	return contextMenu;
}