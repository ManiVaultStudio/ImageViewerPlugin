#include "CanvasWidget.h"
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
#include <QKeyEvent>

CanvasWidget::CanvasWidget(QWidget* parent) :
	QOpenGLWidget(parent),
	QOpenGLFunctions(),
	_imageViewerPlugin(nullptr),
	_renderer(new Renderer(this)),
	_openglDebugLogger(std::make_unique<QOpenGLDebugLogger>()),
	_keys()
{
}

void CanvasWidget::initialize(ImageViewerPlugin* imageViewerPlugin)
{
	_imageViewerPlugin = imageViewerPlugin;

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

CanvasWidget::~CanvasWidget()
{
	_renderer->destroy();
}

bool CanvasWidget::eventFilter(QObject* target, QEvent* event)
{
	switch (event->type())
	{
		case QEvent::KeyPress:
		{
			auto keyEvent = static_cast<QKeyEvent*>(event);

			if (!keyEvent->isAutoRepeat()) {
				if (keyEvent->key() == Qt::Key_Space) {
					_keys |= Qt::Key_Space;
					_renderer->setInteractionMode(InteractionMode::Navigation);
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
					_renderer->setInteractionMode(InteractionMode::LayerEditing);
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
		_imageViewerPlugin->getLayersModel().dispatchEventToSelectedLayer(event);

	return QWidget::eventFilter(target, event);
}

void CanvasWidget::zoomExtents()
{
	auto root = _imageViewerPlugin->getLayersModel().getLayer(QModelIndex());

	if (root != nullptr)
		_renderer->zoomToRectangle(root->getBoundingRectangle());

	update();
}

void CanvasWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	makeCurrent();
	
#ifdef _DEBUG
	_openglDebugLogger->initialize();
#endif

	//_renderer->zoomToRectangle(QRectF(-40, -40, 80, 80));

	doneCurrent();
}

void CanvasWidget::paintGL()
{
	try {
		auto& layersModel = _imageViewerPlugin->getLayersModel();

		QPainter painter;

		painter.begin(this);

		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		drawBackground(&painter);

		painter.beginNativePainting();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto root = layersModel.getLayer(QModelIndex());

		if (root)
			root->render(_renderer->getProjectionMatrix() * _renderer->getViewMatrix());

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

void CanvasWidget::drawBackground(QPainter* painter)
{
	painter->setPen(Qt::NoPen);
	painter->setBrush(_backgroundGradient);
	painter->drawRect(rect());
}

QMenu* CanvasWidget::getContextMenu()
{
	auto contextMenu = _renderer->getContextMenu();
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