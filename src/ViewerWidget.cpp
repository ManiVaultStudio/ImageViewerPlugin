#include "ViewerWidget.h"
#include "ImageViewerPlugin.h"
#include "Renderer.h"
#include "LayersModel.h"
#include "LayerNode.h"

#include <vector>
#include <algorithm>

#include <QItemSelectionModel>
#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QGuiApplication>
#include <QOpenGLDebugLogger>

ViewerWidget::ViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLWidget(imageViewerPlugin),
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_renderer(new Renderer(this)),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	LayerNode::imageViewerPlugin = _imageViewerPlugin;
	Renderable::renderer = _renderer;
	Prop::renderer = _renderer;

	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

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

ViewerWidget::~ViewerWidget()
{
	_renderer->destroy();
}

void ViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	makeCurrent();
	
	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glCullFace(GL_BACK);

	//glDisable(GL_DEPTH_TEST);

	//glDepthMask(GL_FALSE);

	//glEnable(GL_MULTISAMPLE);

#ifdef _DEBUG
	_openglDebugLogger->initialize();
#endif

	_renderer->zoomToRectangle(QRectF(-40, -40, 80, 80));

	doneCurrent();
}

void ViewerWidget::paintGL()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto root = _imageViewerPlugin->layersModel().getLayer(QModelIndex());

	if (root)
		root->render(_renderer->projectionMatrix() * _renderer->viewMatrix());

#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		qDebug() << message;
#endif
}

void ViewerWidget::publishSelection()
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

QMenu* ViewerWidget::contextMenu()
{
	auto contextMenu = _renderer->contextMenu();
	/*
	if (_imageViewerPlugin->imageCollectionType() == ImageCollectionType::Stack) {
		auto* createSubsetFromSelectionAction = new QAction("Create subset from selection");

		createSubsetFromSelectionAction->setEnabled(_imageViewerPlugin->noSelectedPixels() > 0);

		connect(createSubsetFromSelectionAction, &QAction::triggered, _imageViewerPlugin, &ImageViewerPlugin::createSubsetFromSelection);

		contextMenu->addSeparator();
		contextMenu->addAction(createSubsetFromSelectionAction);
	}
	*/

	return contextMenu;
}