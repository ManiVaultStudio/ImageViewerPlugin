#include "ViewerWidget.h"
#include "ImageViewerPlugin.h"
#include "Renderer.h"
#include "LayersModel.h"
#include "ClusterData.h"
#include "RenderNode.h"

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
	RenderNode::renderer = _renderer;
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

	/*
	QObject::connect(layersModel(), &DatasetsModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int> &roles ) {
		_renderer->render();
	});

	QObject::connect(&layersModel()->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](const QModelIndex& current, const QModelIndex& previous) {
		if (previous.isValid()) {
			const auto name = layersModel()->data(previous.row(), DatasetsModel::Columns::Name, Qt::EditRole).toString();

			_renderer->removeActor(name);
		}

		if (current.isValid()) {
			const auto name = layersModel()->data(current.row(), DatasetsModel::Columns::Name, Qt::EditRole).toString();
			const auto size = layersModel()->data(current.row(), DatasetsModel::Columns::Size, Qt::EditRole).toSize();
			
			_renderer->addActor<ImageDatasetActor>(_renderer, name, layersModel());
			_renderer->zoomToRectangle(QRectF(QPointF(), size));
		}
	});
	*/
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

	_renderer->init();

#ifdef _DEBUG
	_openglDebugLogger->initialize();
#endif

	doneCurrent();
}

void ViewerWidget::paintGL() {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_renderer->render();

#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		qDebug() << message;
#endif
}

void ViewerWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	_renderer->keyPressEvent(keyEvent);

	QOpenGLWidget::keyPressEvent(keyEvent);
}

void ViewerWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	_renderer->keyReleaseEvent(keyEvent);

	QOpenGLWidget::keyReleaseEvent(keyEvent);
}

void ViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	/*
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
	*/
	QOpenGLWidget::mousePressEvent(mouseEvent);
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (mouseEvent->button() == Qt::RightButton && _renderer->allowsContextMenu()) {
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}

	_renderer->mouseReleaseEvent(mouseEvent);

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	_renderer->mouseMoveEvent(mouseEvent);

	QOpenGLWidget::mouseMoveEvent(mouseEvent);
}

void ViewerWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	_renderer->mouseWheelEvent(wheelEvent);
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