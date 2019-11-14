#include "ImageViewerWidget.h"
#include "ImageViewerPlugin.h"

#include "PointsPlugin.h"

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

// Panning and zooming inspired by: https://community.khronos.org/t/opengl-compound-zoom-and-pan-effect/72565/7

const std::string vertexShaderSource =
#include "Vertex.glsl"
;

const std::string imageFragmentShaderSource =
#include "ImageFragment.glsl"
;

const std::string computeOverlayFragmentShaderSource =
#include "ComputeOverlayFragment.glsl"
;

const std::string overlayFragmentShaderSource =
#include "OverlayFragment.glsl"
;

const std::string selectionFragmentShaderSource =
#include "SelectionFragment.glsl"
;

const std::string selectionGeometryVertexShaderSource =
#include "SelectionGeometryVertex.glsl"
;

const std::string selectionGeometryFragmentShaderSource =
#include "SelectionGeometryFragment.glsl"
;

const std::string selectionBoundsFragmentShaderSource =
#include "SelectionBoundsFragment.glsl"
;

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

ImageViewerWidget::ImageViewerWidget(ImageViewerPlugin* imageViewerPlugin) :
	QOpenGLFunctions(),
	_imageViewerPlugin(imageViewerPlugin),

	// QT OpenGL
	_imageTexture(),
	_selectionTexture(),
	_imageShaderProgram(),
	_pixelSelectionShaderProgram(),
	_overlayShaderProgram(),
	_selectionShaderProgram(),
	_selectionBoundsShaderProgram(),
	_pixelSelectionFBO(),
	_imageQuadVBO(),
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
	_brushRadius(50.f),
	_brushRadiusDelta(2.0f),
	_pointSelectionColor(1.f, 0.f, 0.f, 0.8f),
	_pixelSelectionColor(1.f, 0.6f, 0.f, 0.4f),
	_selectionOutlineColor(255, 0, 0, 255),
	_selectionBoundsColor(1.0f, 0.0f, 0.f, 1.0f),
	_selectedPointIds(),
	_selectionBounds{ 0, 0, 0, 0 },
	_noSelectedPixels(0),
	_ignorePaintGL(false),
	_window(1.0f),
	_level(0.5f)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	setFocusPolicy(Qt::StrongFocus);

	setMouseTracking(true);

	connect(_imageViewerPlugin, &ImageViewerPlugin::currentDatasetChanged, this, &ImageViewerWidget::onCurrentDatasetChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::currentImageIdChanged, this, &ImageViewerWidget::onCurrentImageIdChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::displayImageChanged, this, &ImageViewerWidget::onDisplayImageChanged);
	connect(_imageViewerPlugin, &ImageViewerPlugin::selectionImageChanged, this, &ImageViewerWidget::onSelectionImageChanged);

	QSurfaceFormat surfaceFormat;

	surfaceFormat.setRenderableType(QSurfaceFormat::OpenGL);
	//surfaceFormat.setProfile(QSurfaceFormat::CoreProfile);
	//surfaceFormat.setVersion(4, 3);
	surfaceFormat.setSamples(16);
	//surfaceFormat.setDepthBufferSize(24);
	//surfaceFormat.setStencilBufferSize(8);

	setFormat(surfaceFormat);
}

ImageViewerWidget::~ImageViewerWidget()
{
	makeCurrent();

	_imageTexture.reset();
	_selectionTexture.reset();
	_imageShaderProgram.reset();
	_pixelSelectionShaderProgram.reset();
	_overlayShaderProgram.reset();
	_selectionShaderProgram.reset();
	_pixelSelectionFBO.reset();

	_selectionGeometryShaderProgram.reset();

	if (_imageQuadVBO.isCreated())
		_imageQuadVBO.destroy();

	doneCurrent();
}

void ImageViewerWidget::enableSelection(const bool& enable)
{
	_selecting = enable;

	update();
}

void ImageViewerWidget::initializeGL()
{
	qDebug() << "Initializing OpenGL";

	initializeOpenGLFunctions();

	_imageQuadVBO.create();
	_imageQuadVBO.setUsagePattern(QOpenGLBuffer::DynamicDraw);

	_imageTexture					= std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
	_selectionTexture				= std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
	_imageShaderProgram				= std::make_unique<QOpenGLShaderProgram>();
	_overlayShaderProgram			= std::make_unique<QOpenGLShaderProgram>();
	_pixelSelectionShaderProgram	= std::make_unique<QOpenGLShaderProgram>();
	_selectionShaderProgram			= std::make_unique<QOpenGLShaderProgram>();
	_selectionGeometryShaderProgram = std::make_unique<QOpenGLShaderProgram>();
	_selectionBoundsShaderProgram	= std::make_unique<QOpenGLShaderProgram>();

	_imageShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_imageShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, imageFragmentShaderSource.c_str());
	_imageShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_imageShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	_imageShaderProgram->link();

	_pixelSelectionShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_pixelSelectionShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, computeOverlayFragmentShaderSource.c_str());
	_pixelSelectionShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_pixelSelectionShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	_pixelSelectionShaderProgram->link();

	_overlayShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_overlayShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, overlayFragmentShaderSource.c_str());
	_overlayShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_overlayShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	_overlayShaderProgram->link();

	_selectionShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource.c_str());
	_selectionShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionFragmentShaderSource.c_str());
	_selectionShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_selectionShaderProgram->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
	_selectionShaderProgram->link();

	_selectionGeometryShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionGeometryVertexShaderSource.c_str());
	_selectionGeometryShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionGeometryFragmentShaderSource.c_str());
	_selectionGeometryShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_selectionGeometryShaderProgram->link();

	_selectionBoundsShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, selectionGeometryVertexShaderSource.c_str());
	_selectionBoundsShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, selectionBoundsFragmentShaderSource.c_str());
	_selectionBoundsShaderProgram->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
	_selectionBoundsShaderProgram->link();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);
}

void ImageViewerWidget::resizeGL(int w, int h)
{
	qDebug() << "Resizing image viewer";

	zoomExtents();
}

void ImageViewerWidget::paintGL() {

	if (_ignorePaintGL)
		return;

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!initialized())
		return;
	
	if (_imageQuadVBO.bind()) {
		auto modelViewProjection = projection() * modelView();

		if (_imageShaderProgram->isLinked() && _imageTexture->isCreated()) {
			if (_imageShaderProgram->bind()) {
				_imageShaderProgram->setUniformValue("imageTexture", 0);
				_imageShaderProgram->setUniformValue("matrix", modelViewProjection);

				const auto imageMin			= static_cast<float>(_imageMin);
				const auto imageMax			= static_cast<float>(_imageMax);
				const auto maxWindow		= static_cast<float>(imageMax - imageMin);
				const auto level			= std::clamp(imageMin + (_level * maxWindow), imageMin, imageMax);
				const auto window			= std::clamp(_window * maxWindow, imageMin, imageMax);
				const auto minPixelValue	= std::clamp(level - (window / 2.0f), imageMin, imageMax);
				const auto maxPixelValue	= std::clamp(level + (window / 2.0f), imageMin, imageMax);
				
				_imageShaderProgram->setUniformValue("minPixelValue", minPixelValue);
				_imageShaderProgram->setUniformValue("maxPixelValue", maxPixelValue);

				_imageShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
				_imageShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
				_imageShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
				_imageShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

				_imageTexture->bind();

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				_imageTexture->release();

				_imageShaderProgram->release();
			}
		}
		
		if (_overlayShaderProgram->isLinked()) {
			auto translate = QMatrix4x4();

			translate.translate(0.0f, 0.0f, 1.0f);

			if (_overlayShaderProgram->bind()) {
				_overlayShaderProgram->setUniformValue("overlayTexture", 0);
				_overlayShaderProgram->setUniformValue("matrix", modelViewProjection * translate);
				_overlayShaderProgram->setUniformValue("color", _pixelSelectionColor);
				_overlayShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
				_overlayShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
				_overlayShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
				_overlayShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

				glBindTexture(GL_TEXTURE_2D, _pixelSelectionFBO->texture());
				
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				_overlayShaderProgram->release();
			}
		}
		
		if (_selectionShaderProgram->isLinked() && _selectionTexture->isCreated()) {
			modelViewProjection.translate(0.0f, 0.0f, 2.0f);

			if (_selectionShaderProgram->bind()) {
				_selectionShaderProgram->setUniformValue("selectionTexture", 0);
				_selectionShaderProgram->setUniformValue("matrix", modelViewProjection);
				_selectionShaderProgram->setUniformValue("color", _pointSelectionColor);
				_selectionShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
				_selectionShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
				_selectionShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
				_selectionShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

				_selectionTexture->bind();

				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

				_selectionTexture->release();

				_selectionShaderProgram->release();
			}
		}
		
		_imageQuadVBO.release();
	}
	
	if (_interactionMode == InteractionMode::Selection) {
		if (_selectionGeometryShaderProgram->isLinked() && _selectionGeometryShaderProgram->bind()) {
			QMatrix4x4 transform;

			transform.ortho(rect());

			_selectionGeometryShaderProgram->setUniformValue("matrix", transform);

			const auto vertexLocation = _selectionGeometryShaderProgram->attributeLocation("vertex");
			
			_selectionGeometryShaderProgram->enableAttributeArray(vertexLocation);

			drawSelectionOutline();

			_selectionGeometryShaderProgram->disableAttributeArray(vertexLocation);
		}
		
		_selectionGeometryShaderProgram->release();
	}

	if (_selectionBoundsShaderProgram->isLinked() && _selectionBoundsShaderProgram->bind()) {
		const auto transform = projection() * modelView();

		_selectionBoundsShaderProgram->setUniformValue("matrix", transform);
		_selectionBoundsShaderProgram->setUniformValue("color", _selectionBoundsColor);

		drawSelectionBounds();

		_selectionBoundsShaderProgram->release();
	}
}

void ImageViewerWidget::onDisplayImageChanged(std::unique_ptr<QImage>& displayImage)
{
	if (!isValid())
		return;
	
	makeCurrent();

	_ignorePaintGL = true;

	qDebug() << "Display image changed" << *displayImage.get();

	auto imageSizeChanged = false;

	if (_displayImage.get() == nullptr)
		imageSizeChanged = true;
	else
		imageSizeChanged = displayImage->width() != _displayImage->width() || displayImage->height() != _displayImage->height();

	_displayImage.swap(displayImage);
	
	std::uint16_t* pixels = (std::uint16_t*)_displayImage->bits();

	const auto noPixels = _displayImage->width() * _displayImage->height();

	auto test = std::vector<std::uint16_t>(pixels, pixels + noPixels * 4);

	_imageMin = std::numeric_limits<std::uint16_t>::max();
	_imageMax = std::numeric_limits<std::uint16_t>::min();

	for (std::uint32_t y = 0; y < _displayImage->height(); y++)
	{
		for (std::uint32_t x = 0; x < _displayImage->width(); x++)
		{
			const auto pixelId = y * _displayImage->width() + x;

			for (int c = 0; c < 3; c++)
			{
				const auto channel = reinterpret_cast<std::uint16_t*>(_displayImage->bits())[pixelId * 4 + c];

				if (channel < _imageMin)
					_imageMin = channel;

				if (channel > _imageMax)
					_imageMax = channel;
			}
		}
	}

	_imageTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));

	//_imageTexture->create();
	_imageTexture->setSize(_displayImage->size().width(), _displayImage->size().height());
	_imageTexture->setFormat(QOpenGLTexture::RGBA16_UNorm);
	_imageTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
	_imageTexture->allocateStorage();
	_imageTexture->setData(QOpenGLTexture::PixelFormat::RGBA, QOpenGLTexture::PixelType::UInt16, _displayImage->bits());

	resetWindowLevel();

	if (imageSizeChanged) {
		createImageQuad();
		zoomExtents();

		const auto brushRadius = 0.05f * static_cast<float>(std::min(_displayImage->size().width(), _displayImage->size().height()));
		
		setBrushRadius(brushRadius);
		setBrushRadiusDelta(0.2f * brushRadius);

		_pixelSelectionFBO = std::make_unique<QOpenGLFramebufferObject>(_displayImage->width(), _displayImage->height());
	}

	_ignorePaintGL = false;

	doneCurrent();

	update();
}

void ImageViewerWidget::onSelectionImageChanged(std::unique_ptr<QImage>& selectionImage)
{
	if (!isValid())
		return;

	makeCurrent();

	qDebug() << "Selection image changed";

	_selectionImage.swap(selectionImage);

	_selectionTexture.reset(new QOpenGLTexture(*_selectionImage.get()));
	_selectionTexture->setMinMagFilters(QOpenGLTexture::Filter::Nearest, QOpenGLTexture::Filter::Nearest);

	const auto numericMin = std::numeric_limits<std::uint32_t>::min();
	const auto numericMax = std::numeric_limits<std::uint32_t>::max();

	_selectionBounds[0] = numericMax;
	_selectionBounds[1] = numericMin;
	_selectionBounds[2] = numericMax;
	_selectionBounds[3] = numericMin;

	_noSelectedPixels = 0;

	for (std::uint32_t y = 0; y < _selectionImage->height(); y++) {
		for (std::uint32_t x = 0; x < _selectionImage->width(); x++) {
			if (_selectionImage->pixelColor(x, y).red() > 0) {
				_noSelectedPixels++;

				if (x < _selectionBounds[0])
					_selectionBounds[0] = x;

				if (x > _selectionBounds[1])
					_selectionBounds[1] = x;

				if (y < _selectionBounds[2])
					_selectionBounds[2] = y;

				if (y > _selectionBounds[3])
					_selectionBounds[3] = y;
			}
		}
	}

	doneCurrent();

	update();
}

void ImageViewerWidget::onCurrentDatasetChanged(const QString& currentDataset)
{
	enableSelection(false);
}

void ImageViewerWidget::onCurrentImageIdChanged(const std::int32_t& currentImageId)
{
	enableSelection(false);

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
		if (_interactionMode == InteractionMode::Selection) {
			switch (keyEvent->key())
			{
				case Qt::Key::Key_R:
					setSelectionType(SelectionType::Rectangle);
					break;

				case Qt::Key::Key_B:
					setSelectionType(SelectionType::Brush);
					break;

				case Qt::Key::Key_L:
					setSelectionType(SelectionType::Lasso);
					break;

				case Qt::Key::Key_Shift:
					setSelectionModifier(SelectionModifier::Add);
					break;

				case Qt::Key::Key_Control:
					setSelectionModifier(SelectionModifier::Remove);
					break;

				case Qt::Key::Key_Space:
					setInteractionMode(InteractionMode::Navigation);
					break;

				default:
					break;
			}
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
				if (selectionType() != SelectionType::Brush) {
					setSelectionModifier(SelectionModifier::Replace);
				}

				break;
			}

			case Qt::Key::Key_Space:
				setInteractionMode(InteractionMode::Selection);
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

	switch (mouseEvent->button())
	{
		case Qt::LeftButton:
		{
			_mousePosition = mouseEvent->pos();

			switch (_interactionMode)
			{
				case InteractionMode::Navigation:
					break;
				case InteractionMode::Selection:
				{
					if (_imageViewerPlugin->selectable()) {
						/*
						if (_selectionModifier == SelectionModifier::Replace) {
							qDebug() << "Reset selection";

							_imageViewerPlugin->setSelection(Indices());
						}
						*/

						resetPixelSelection();

						_mousePositions.clear();

						_initialMousePosition = _mousePosition;

						enableSelection(true);
					}

					break;
				}
				case InteractionMode::WindowLevel:
					break;
				default:
					break;
			}

			break;
		}

		case Qt::RightButton:
		{
			setInteractionMode(InteractionMode::WindowLevel);
			break;
		}

		default:
			break;
	}
}

void ImageViewerWidget::mouseMoveEvent(QMouseEvent* mouseEvent) {

	if (!initialized())
		return;

	//qDebug() << "Mouse move event";

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
					if (_imageViewerPlugin->selectable()) {
						_mousePositions.push_back(mouseEvent->pos());

						updatePixelSelection();
					}
					
					break;
				}

				default:
					break;
			}

			update();

			break;
		}

		case Qt::RightButton:
		{
			const auto worldPos		= screenToWorld(_mousePosition);
			const auto deltaWindow	= (mouseEvent->pos().x() - _mousePosition.x()) / 150.f;
			const auto deltaLevel	= -(mouseEvent->pos().y() - _mousePosition.y()) / 150.f;
			const auto window		= std::clamp(_window + deltaWindow, 0.0f, 1.0f);
			const auto level		= std::clamp(_level + deltaLevel, 0.0f, 1.0f);

			setWindowLevel(window, level);

			_mousePositions.push_back(_mousePosition);

			break;
		}

		default:
			break;
	}

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

	switch (_interactionMode)
	{
		case InteractionMode::Navigation:
		{
			QWidget::setCursor(Qt::OpenHandCursor);
			break;
		}

		case InteractionMode::Selection:
		{
			if (_imageViewerPlugin->selectable()) {
				if (_selecting) {
					enableSelection(false);
					modifySelection();
					commitSelection();
				}
			}
			break;
		}

		case InteractionMode::WindowLevel:
		{
			setInteractionMode(InteractionMode::Selection);
			break;
		}

		default:
			break;
	}

	_mousePositions.clear();

	update();

	QOpenGLWidget::mouseReleaseEvent(mouseEvent);
}

void ImageViewerWidget::wheelEvent(QWheelEvent* wheelEvent) {

	if (!initialized())
		return;

	qDebug() << "Mouse wheel event";

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
					setBrushRadius(_brushRadius + _brushRadiusDelta);
				}
				else {
					setBrushRadius(_brushRadius - _brushRadiusDelta);
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
	
	qDebug() << "Zoom extents";

	resetView();

	const auto factorX = (width() - _margin) / static_cast<float>(_displayImage->width());
	const auto factorY = (height() - _margin) / static_cast<float>(_displayImage->height());

	zoom(factorX < factorY ? factorX : factorY);
	pan(_zoom * -QPointF(_displayImage->width() / 2.0f, _displayImage->height() / 2.0f));

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

std::uint16_t ImageViewerWidget::windowLevel(const float& min, const float& max, const float& windowNorm, const float& levelNorm, const float& pointValue)
{
	const auto maxWindow = max - min;

	const auto level			= std::clamp(min + (levelNorm * maxWindow), min, max);
	const auto window			= std::clamp(windowNorm * maxWindow, min, max);
	const auto minPixelValue	= std::clamp(level - (window / 2.0f), min, max);
	const auto maxPixelValue	= std::clamp(level + (window / 2.0f), min, max);
	const auto fraction			= pointValue - minPixelValue;
	const auto range			= maxPixelValue - minPixelValue;

	if (range == 0)
		return 0;

	return static_cast<std::uint16_t>(std::clamp(fraction / range, 0.0f, 1.0f) * std::numeric_limits<std::uint16_t>::max());
}

bool ImageViewerWidget::initialized()
{
	return _displayImage.get() != nullptr;
}

QVector3D ImageViewerWidget::screenToWorld(const QPoint& screenPoint) const
{
	return QVector3D(screenPoint.x(), height() - screenPoint.y(), 0).unproject(modelView(), projection(), QRect(0, 0, width(), height()));
}

void ImageViewerWidget::createImageQuad()
{
	qDebug() << "Create image quad" << *_displayImage.get();

	const float width	= static_cast<float>(_displayImage->width());
	const float height	= static_cast<float>(_displayImage->height());

	const float coordinates[4][3] = {
	  { width, height, 0.0f },
	  { 0.0f, height, 0.0f },
	  { 0.0f, 0.0f, 0.0f },
	  { width, 0.0f, 0.0f }
	};
	
	QVector<GLfloat> vertexData;

	for (int j = 0; j < 4; ++j)
	{
		// vertex position
		vertexData.append(1.0 * coordinates[j][0]);
		vertexData.append(1.0 * coordinates[j][1]);
		vertexData.append(1.0 * coordinates[j][2]);

		// texture coordinate
		vertexData.append(j == 0 || j == 3);
		vertexData.append(j == 2 || j == 3);
	}

	_imageQuadVBO.bind();
	_imageQuadVBO.allocate(vertexData.constData(), vertexData.count() * sizeof(GLfloat));
	_imageQuadVBO.release();
}

void ImageViewerWidget::updatePixelSelection()
{
	//qDebug() << "Update selection" << selectionTypeName(_selectionType);

	makeCurrent();
	
	if (!_pixelSelectionFBO->bind())
		return;

	glViewport(0, 0, _displayImage->width(), _displayImage->height());

	QMatrix4x4 transform;

	transform.ortho(0.0f, _displayImage->width(), 0.0f, _displayImage->height(), -1.0f, +1.0f);

	if (_imageQuadVBO.bind()) {
		if (_pixelSelectionShaderProgram->bind()) {
			glBindTexture(GL_TEXTURE_2D, _pixelSelectionFBO->texture());

			_pixelSelectionShaderProgram->setUniformValue("pixelSelectionTexture", 0);
			_pixelSelectionShaderProgram->setUniformValue("matrix", transform);
			_pixelSelectionShaderProgram->setUniformValue("selectionType", static_cast<int>(_selectionType));
			_pixelSelectionShaderProgram->setUniformValue("imageSize", static_cast<float>(_displayImage->size().width()), static_cast<float>(_displayImage->size().height()));
				
			switch (_selectionType)
			{
				case SelectionType::Rectangle:
				{
					const auto rectangleTopLeft			= screenToWorld(_mousePositions.front());
					const auto rectangleBottomRight		= screenToWorld(_mousePositions.back());
					const auto rectangleTopLeftUV		= QVector2D(rectangleTopLeft.x() / static_cast<float>(_displayImage->width()), rectangleTopLeft.y() / static_cast<float>(_displayImage->height()));
					const auto rectangleBottomRightUV	= QVector2D(rectangleBottomRight.x() / static_cast<float>(_displayImage->width()), rectangleBottomRight.y() / static_cast<float>(_displayImage->height()));

					auto rectangleUV	= std::make_pair(rectangleTopLeftUV, rectangleBottomRightUV);
					auto topLeft		= QVector2D(rectangleTopLeftUV.x(), rectangleTopLeftUV.y());
					auto bottomRight	= QVector2D(rectangleBottomRightUV.x(), rectangleBottomRightUV.y());

					if (rectangleBottomRightUV.x() < rectangleTopLeftUV.x()) {
						topLeft.setX(rectangleBottomRightUV.x());
						bottomRight.setX(rectangleTopLeftUV.x());
					}
							
					if (rectangleBottomRightUV.y() < rectangleTopLeftUV.y()) {
						topLeft.setY(rectangleBottomRightUV.y());
						bottomRight.setY(rectangleTopLeftUV.y());
					}
							
					_pixelSelectionShaderProgram->setUniformValue("rectangleTopLeft", topLeft);
					_pixelSelectionShaderProgram->setUniformValue("rectangleBottomRight", bottomRight);

					break;
				}

				case SelectionType::Brush:
				{
					const auto currentMousePosition		= _mousePositions[_mousePositions.size() - 1]; 
					const auto previousMousePosition	= _mousePositions.size() > 1 ? _mousePositions[_mousePositions.size() - 2] : currentMousePosition;
					const auto brushCenter				= screenToWorld(currentMousePosition);
					const auto previousBrushCenter		= screenToWorld(previousMousePosition);

					_pixelSelectionShaderProgram->setUniformValue("previousBrushCenter", previousBrushCenter.x(), previousBrushCenter.y());
					_pixelSelectionShaderProgram->setUniformValue("currentBrushCenter", brushCenter.x(), brushCenter.y());
					_pixelSelectionShaderProgram->setUniformValue("brushRadius", _brushRadius);

					break;
				}

				case SelectionType::Lasso:
				{
					QList<QVector2D> mousePositions;
					
					mousePositions.reserve(_mousePositions.size());

					for (const auto p : _mousePositions) {
						mousePositions.push_back(QVector2D(screenToWorld(p).x(), screenToWorld(p).y()));
					}

					_pixelSelectionShaderProgram->setUniformValueArray("points", &mousePositions[0], _mousePositions.size());
					_pixelSelectionShaderProgram->setUniformValue("noPoints", static_cast<int>(_mousePositions.size()));

					break;
				}

				default:
					break;
			}
				
			_pixelSelectionShaderProgram->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
			_pixelSelectionShaderProgram->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
			_pixelSelectionShaderProgram->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
			_pixelSelectionShaderProgram->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			_pixelSelectionShaderProgram->release();
		}

		_imageQuadVBO.release();
	}

	_pixelSelectionFBO->release();

	doneCurrent();

	update();
}

void ImageViewerWidget::resetPixelSelection()
{
	makeCurrent();

	if (_pixelSelectionFBO->bind()) {
		glViewport(0, 0, _displayImage->width(), _displayImage->height());
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	doneCurrent();

	update();
}

void ImageViewerWidget::modifySelection()
{
	qDebug() << "Modify selection";
	
	makeCurrent();

	const auto image = _pixelSelectionFBO->toImage();

	doneCurrent();

	resetPixelSelection();

	auto pixelCoordinates = std::vector<std::pair<std::uint32_t, std::uint32_t>>();

	pixelCoordinates.reserve(image.width() * image.height());

	for (std::uint32_t y = 0; y < image.height(); y++) {
		for (std::uint32_t x = 0; x < image.width(); x++) {
			if (image.pixelColor(x, y).red() > 0) {
				pixelCoordinates.push_back(std::make_pair(x, y));
			}
		}
	}

	_imageViewerPlugin->selectPixels(pixelCoordinates, _selectionModifier);

	update();
}

void ImageViewerWidget::clearSelection()
{
	qDebug() << "Clear selection";

	modifySelection();
	commitSelection();
}

void ImageViewerWidget::commitSelection()
{
	qDebug() << "Commit selection to core";

	resetPixelSelection();
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
	auto* resetWindowLevelAction = new QAction("Reset window/level");

	zoomToExtentsAction->setToolTip("Zoom to the boundaries of the image");
	resetWindowLevelAction->setToolTip("Reset window/level to default values");

	connect(zoomToExtentsAction, &QAction::triggered, this, &ImageViewerWidget::zoomExtents);
	connect(resetWindowLevelAction, &QAction::triggered, this, &ImageViewerWidget::resetWindowLevel);

	viewMenu->addAction(zoomToExtentsAction);
	viewMenu->addAction(resetWindowLevelAction);

	return viewMenu;
}

QMenu* ImageViewerWidget::selectionMenu()
{
	auto* selectionMenu = new QMenu("Selection");

	auto* rectangleSelectionAction = new QAction("Rectangle");
	auto* brushSelectionAction = new QAction("Brush");
	auto* freehandSelectionAction = new QAction("Freehand", this);
	auto* clearSelectionAction = new QAction("Clear");

	connect(rectangleSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Rectangle);  });
	connect(brushSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Brush);  });
	connect(freehandSelectionAction, &QAction::triggered, [this]() { setSelectionType(SelectionType::Lasso);  });
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

void ImageViewerWidget::setupTextures()
{
	qDebug() << "Setup textures";

	setupTexture(_imageTexture.get(), QOpenGLTexture::TextureFormat::RGB16_UNorm);
	setupTexture(_selectionTexture.get(), QOpenGLTexture::TextureFormat::R8_UNorm, QOpenGLTexture::Filter::Nearest);
}

void ImageViewerWidget::setupTexture(QOpenGLTexture* openGltexture, const QOpenGLTexture::TextureFormat& textureFormat, const QOpenGLTexture::Filter& filter /*= QOpenGLTexture::Filter::Linear*/)
{
	qDebug() << "Setup texture" << *_displayImage.get();

	openGltexture->destroy();
	openGltexture->create();
	openGltexture->setSize(_displayImage->width(), _displayImage->height(), 1);
	openGltexture->setFormat(textureFormat);
	openGltexture->allocateStorage();
	openGltexture->setMinMagFilters(filter, filter);
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

	if (selectionType == SelectionType::Brush) {
		//_selectionModifier = SelectionModifier::Add;
	}
	else {
		//_selectionModifier = SelectionModifier::Replace;
	}

	update();

	emit selectionTypeChanged();
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

	if (selectionType() == SelectionType::Brush && selectionModifier == SelectionModifier::Replace) {
	}
	else {
		_selectionModifier = selectionModifier;

		emit selectionModifierChanged();
	}
}

void ImageViewerWidget::setBrushRadius(const float& brushRadius)
{
	const auto boundBrushRadius = qBound(1.0f, 10000.f, brushRadius);

	if (boundBrushRadius == _brushRadius)
		return;

	_brushRadius = boundBrushRadius;

	qDebug() << "Set brush radius" << brushRadius;

	update();
}

void ImageViewerWidget::setBrushRadiusDelta(const float& brushRadiusDelta)
{
	const auto boundBrushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	if (boundBrushRadiusDelta == _brushRadiusDelta)
		return;

	_brushRadiusDelta = qBound(0.001f, 10000.f, brushRadiusDelta);

	qDebug() << "Set brush radius delta" << _brushRadiusDelta;

	update();
}

std::pair<float, float> ImageViewerWidget::windowLevel() const
{
	return std::make_pair(_window, _level);
}

void ImageViewerWidget::setWindowLevel(const float& window, const float& level)
{
	if (window == _window && level == _level)
		return;

	_window = std::clamp(window, 0.01f, 1.0f);
	_level	= std::clamp(level, 0.01f, 1.0f);

	qDebug() << "Set window/level" << _window << _level;

	update();
}

void ImageViewerWidget::resetWindowLevel()
{
	_window = 1.0;
	_level = 0.5;

	update();
}

void ImageViewerWidget::drawSelectionOutlineRectangle(const QPoint& start, const QPoint& end)
{
	const GLfloat vertexCoordinates[] = {
	  start.x(), start.y(), 0.0f,
	  end.x(), start.y(), 0.0f,
	  end.x(), end.y(), 0.0f,
	  start.x(), end.y(), 0.0f
	};

	const auto vertexLocation = _selectionGeometryShaderProgram->attributeLocation("vertex");

	_selectionGeometryShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates, 3);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	_selectionGeometryShaderProgram->disableAttributeArray(vertexLocation);
}

void ImageViewerWidget::drawSelectionOutlineBrush()
{
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
		vertexCoordinates[s * 3 + 2] = 0;
	}

	const auto vertexLocation = _selectionGeometryShaderProgram->attributeLocation("vertex");

	_selectionGeometryShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glDrawArrays(GL_LINE_LOOP, 0, noSegments);

	_selectionGeometryShaderProgram->disableAttributeArray(vertexLocation);
}

void ImageViewerWidget::drawSelectionOutlineLasso()
{
	std::vector<GLfloat> vertexCoordinates;

	vertexCoordinates.resize(_mousePositions.size() * 3);

	for (std::size_t p = 0; p < _mousePositions.size(); p++) {
		const auto mousePosition = _mousePositions[p];

		vertexCoordinates[p * 3 + 0] = mousePosition.x();
		vertexCoordinates[p * 3 + 1] = mousePosition.y();
		vertexCoordinates[p * 3 + 2] = 0;
	}

	const auto vertexLocation = _selectionGeometryShaderProgram->attributeLocation("vertex");

	_selectionGeometryShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates.data(), 3);

	glDrawArrays(GL_LINE_LOOP, 0, _mousePositions.size());

	_selectionGeometryShaderProgram->disableAttributeArray(vertexLocation);
}

void ImageViewerWidget::drawSelectionOutline()
{
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

		default:
			break;
	}
}

void ImageViewerWidget::drawSelectionBounds()
{
	if (_noSelectedPixels == 0)
		return;

	qDebug() << "Draw selection bounds";

	const GLfloat boxScreen[4] = {
		_selectionBounds[0], _selectionBounds[1],
		_displayImage->height() - _selectionBounds[2], _displayImage->height() - _selectionBounds[3]
	};

	const GLfloat vertexCoordinates[] = {
		boxScreen[0], boxScreen[3], 0.0f,
		boxScreen[1], boxScreen[3], 0.0f,
		boxScreen[1], boxScreen[2], 0.0f,
		boxScreen[0], boxScreen[2], 0.0f
	};

	const auto vertexLocation = _selectionBoundsShaderProgram->attributeLocation("vertex");

	_selectionBoundsShaderProgram->setAttributeArray(vertexLocation, vertexCoordinates, 3);

	glDrawArrays(GL_LINE_LOOP, 0, 4);

	_selectionBoundsShaderProgram->disableAttributeArray(vertexLocation);
}
