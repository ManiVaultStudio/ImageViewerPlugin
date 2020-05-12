#include "ViewerWidget.h"
#include "ImageViewerPlugin.h"
#include "Renderer.h"
#include "LayersModel.h"
#include "Layer.h"

#include <vector>
#include <algorithm>

#include <QItemSelectionModel>
#include <QSize>
#include <QDebug>
#include <QMenu>
#include <QGuiApplication>
#include <QOpenGLDebugLogger>
#include <QPainter>
#include <QMessageBox>

ViewerWidget::ViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLWidget(imageViewerPlugin),
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),
	_renderer(new Renderer(this)),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>())
{
	Layer::imageViewerPlugin = _imageViewerPlugin;
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

	_backgroundGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	_backgroundGradient.setCenter(0.5, 0.50);
	_backgroundGradient.setFocalPoint(0.5, 0.5);
	_backgroundGradient.setColorAt(0.0, QColor(100, 100, 100));
	_backgroundGradient.setColorAt(0.7, QColor(30, 30, 30));
}

ViewerWidget::~ViewerWidget()
{
	_renderer->destroy();
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

void ViewerWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	QOpenGLWidget::mousePressEvent(mouseEvent);

	_imageViewerPlugin->layersModel().mousePressEvent(mouseEvent);

	/*
	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			if (_renderer->interactionMode() != InteractionMode::Navigation && allowsPixelSelection()) {
				_renderer->setInteractionMode(InteractionMode::Selection);
			}

			break;
		}

		default:
			break;
	}

	_renderer->mousePressEvent(mouseEvent);
	*/
	
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	QOpenGLWidget::mouseReleaseEvent(mouseEvent);

	_imageViewerPlugin->layersModel().mouseReleaseEvent(mouseEvent);

	/*
	if (mouseEvent->button() == Qt::RightButton && _renderer->allowsContextMenu()) {
		contextMenu()->exec(mapToGlobal(mouseEvent->pos()));
	}
	*/
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	QOpenGLWidget::mouseMoveEvent(mouseEvent);

	_imageViewerPlugin->layersModel().mouseMoveEvent(mouseEvent);
}

void ViewerWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	QOpenGLWidget::wheelEvent(wheelEvent);

	_imageViewerPlugin->layersModel().mouseWheelEvent(wheelEvent);
}

void ViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	makeCurrent();
	
#ifdef _DEBUG
	_openglDebugLogger->initialize();
#endif

	_renderer->zoomToRectangle(QRectF(-40, -40, 80, 80));

	doneCurrent();
}

void ViewerWidget::paintGL()
{
	try {
		auto& layersModel = _imageViewerPlugin->layersModel();

		QPainter painter;

		painter.begin(this);

		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		drawBackground(&painter);

		painter.beginNativePainting();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto root = layersModel.getLayer(QModelIndex());

		if (root)
			root->render(_renderer->projectionMatrix() * _renderer->viewMatrix());

		painter.endNativePainting();

		layersModel.paint(&painter);

		painter.end();
	}
	catch (std::exception& e)
	{
		QMessageBox(QMessageBox::Critical, "Rendering failed", e.what());
	}
	catch (...) {
		QMessageBox(QMessageBox::Critical, "Rendering failed", "An unhandled exception occurred");
	}

#ifdef _DEBUG
	for (const QOpenGLDebugMessage& message : _openglDebugLogger->loggedMessages())
		switch (message.severity())
		{
			case QOpenGLDebugMessage::HighSeverity:
				qDebug() << message;
				break;

			default:
				break;
		}
		
#endif
}

void ViewerWidget::drawBackground(QPainter* painter)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(_backgroundGradient);
	painter->drawRect(rect());
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