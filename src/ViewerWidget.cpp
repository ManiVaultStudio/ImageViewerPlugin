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
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>()),
	_keys()
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

	this->installEventFilter(this);
}

ViewerWidget::~ViewerWidget()
{
	_renderer->destroy();
}

bool ViewerWidget::eventFilter(QObject* target, QEvent* event)
{
	switch (event->type())
	{
		case QEvent::KeyPress:
		{
			auto keyEvent = static_cast<QKeyEvent*>(event);

			if (!keyEvent->isAutoRepeat()) {
				if (keyEvent->key() == Qt::Key_Space) {
					_keys |= Qt::Key_Space;
					setCursor(Qt::ClosedHandCursor);
				}
			}

			break;
		}

		case QEvent::KeyRelease:
		{
			auto keyEvent = static_cast<QKeyEvent*>(event);

			if (!keyEvent->isAutoRepeat()) {
				if (keyEvent->key() == Qt::Key_Space) {
					_keys &= ~Qt::Key_Space;
					setCursor(Qt::ArrowCursor);
				}
			}

			break;
		}

		default:
			break;
	}

	if (_keys & Qt::Key_Space)
		_renderer->handleEvent(event);
	else
		_imageViewerPlugin->layersModel().dispatchEventToSelectedLayer(event);

	return QWidget::eventFilter(target, event);
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